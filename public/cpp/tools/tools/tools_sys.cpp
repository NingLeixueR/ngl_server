/*
* Copyright (c) [2020-2025] NingLeixueR
* 
* Project name: ngl_server
* Project URL: https://github.com/NingLeixueR/ngl_server
* 
* This file is part of the ngl_server project and is distributed under the MIT License.
* You may use, modify, and distribute this project under the license, including commercial use,
* but you must retain the original copyright and license notice.
* 
* For license details, see the LICENSE file in the project root:
* https://github.com/NingLeixueR/ngl_server/blob/main/LICENSE
*/
// File overview: Implements system and filesystem helpers for tools.

#include "tools_sys.h"

#include "actor/actor_base/core/nguid.h"
#include "actor/tab/ttab_servers.h"
#include "tools/curl/ncurl.h"
#include "tools/localtime.h"
#include "tools/tab/xml/sysconfig.h"
#include "tools/threadtools.h"
#include "tools_split.h"

#include <chrono>
#include <filesystem>
#include <iostream>
#include <memory>
#include <mutex>
#include <random>
#include <shared_mutex>
#include <system_error>
#include <thread>

namespace ngl
{
	namespace rand_detail
	{
		uint64_t splitmix64(uint64_t ax)
		{
			ax += 0x9e3779b97f4a7c15ULL;
			ax = (ax ^ (ax >> 30)) * 0xbf58476d1ce4e5b9ULL;
			ax = (ax ^ (ax >> 27)) * 0x94d049bb133111ebULL;
			return ax ^ (ax >> 31);
		}

		std::once_flag& seed_once()
		{
			static std::once_flag g_seed_once;
			return g_seed_once;
		}

		std::atomic<uint64_t>& seed_ctr()
		{
			static std::atomic<uint64_t> g_seed_ctr{ 0 };
			return g_seed_ctr;
		}

		uint64_t make_seed()
		{
			std::random_device lrd;
			const uint64_t la = static_cast<uint64_t>(lrd());
			const uint64_t lb = static_cast<uint64_t>(lrd());
			const uint64_t lt = static_cast<uint64_t>(std::chrono::high_resolution_clock::now().time_since_epoch().count());
			return splitmix64(la ^ (lb << 32) ^ lt);
		}

		uint64_t next_seed()
		{
			std::call_once(seed_once(), []()
			{
				seed_ctr().store(make_seed(), std::memory_order_relaxed);
			});
			const uint64_t lval = seed_ctr().fetch_add(0x9e3779b97f4a7c15ULL, std::memory_order_acq_rel);
			const uint64_t ltid = std::hash<std::thread::id>()(std::this_thread::get_id());
			return splitmix64(lval ^ ltid);
		}
	}

	namespace mail_detail
	{
		std::map<std::string, int32_t> g_mailmap;
		std::shared_mutex g_maillock;
		int32_t g_mail_int = localtime::MINUTES_SECOND * 10;
		constexpr std::size_t g_mail_lim = 1024;

		void prune_mail(int32_t anow)
		{
			for (auto liter = g_mailmap.begin(); liter != g_mailmap.end();)
			{
				if ((anow - liter->second) >= g_mail_int)
				{
					liter = g_mailmap.erase(liter);
					continue;
				}
				++liter;
			}
			if (g_mailmap.size() >= g_mail_lim)
			{
				g_mailmap.erase(g_mailmap.begin());
			}
		}
	}

	namespace tools
	{
		std::string time2str(int autc, const char* aformat)
		{
			char lbuf[1024] = { 0 };
			ngl::localtime::time2str(lbuf, 1024, autc, aformat);
			return lbuf;
		}

		const std::string& server_name()
		{
			const tab_servers* ltab = ttab_servers::instance().const_tab();
			if (ltab == nullptr)
			{
				no_core_dump();
				static std::string ltmp;
				return ltmp;
			}
			return ltab->m_name;
		}

		int rand()
		{
			thread_local std::mt19937_64 lgen(rand_detail::next_seed());
			static thread_local std::uniform_int_distribution<int> ldist(0, RAND_MAX);
			return ldist(lgen);
		}

		bool directories_exists(const std::string& apath)
		{
			std::error_code lec;
			return std::filesystem::is_directory(apath, lec);
		}

		bool file_exists(const std::string& apath)
		{
			std::error_code lec;
			return std::filesystem::is_regular_file(apath, lec);
		}

		bool create_dir(const std::string& apath)
		{
			std::error_code lec;
			if (std::filesystem::exists(apath, lec))
			{
				return std::filesystem::is_directory(apath, lec);
			}
			if (lec)
			{
				return false;
			}
			return std::filesystem::create_directories(apath, lec) && !lec;
		}

		bool file_remove(const std::string& afile)
		{
			std::error_code lec;
			return std::filesystem::remove(afile, lec);
		}

		void dir(const std::string& apath, std::vector<std::string>& afiles, bool aiter)
		{
			std::error_code lec;
			if (!std::filesystem::is_directory(apath, lec))
			{
				return;
			}

			if (aiter)
			{
				for (std::filesystem::recursive_directory_iterator liter(apath, std::filesystem::directory_options::skip_permission_denied, lec), lend;
					!lec && liter != lend;
					liter.increment(lec))
				{
					if (liter->is_regular_file(lec))
					{
						afiles.push_back(liter->path().string());
					}
				}
				return;
			}

			for (std::filesystem::directory_iterator liter(apath, std::filesystem::directory_options::skip_permission_denied, lec), lend;
				!lec && liter != lend;
				liter.increment(lec))
			{
				if (liter->is_regular_file(lec))
				{
					afiles.push_back(liter->path().string());
				}
			}
		}

		void no_core_dump(bool anocreate)
		{
			if (!anocreate)
			{
				*(int*)(nullptr) = 19890519;
			}
		}

		std::function<void()> send_mail(const std::string& acontent)
		{
			return [acontent]()
			{
				const int32_t lnow = static_cast<int32_t>(localtime::gettime());
				{
					lock_write(mail_detail::g_maillock);
					if (mail_detail::g_mailmap.size() >= mail_detail::g_mail_lim)
					{
						mail_detail::prune_mail(lnow);
					}
					auto liter = mail_detail::g_mailmap.find(acontent);
					if (liter != mail_detail::g_mailmap.end() && (lnow - liter->second) < mail_detail::g_mail_int)
					{
						return;
					}
					mail_detail::g_mailmap[acontent] = lnow;
				}

				std::cout << "dump_logic()" << std::endl;
				std::shared_ptr<ngl::mail_param> lparm = ngl::ncurl::mail();
				lparm->m_smtp = nconfig.mail().m_smtp;
				lparm->m_email = nconfig.mail().m_email;
				lparm->m_password = nconfig.mail().m_password;
				lparm->m_name = nconfig.mail().m_name;
				lparm->m_title = nconfig.servername();
				lparm->m_content = acontent;
				lparm->m_recvs.emplace_back(std::make_pair("348634371@qq.com", "QQ"));
				lparm->set_wait();
				ngl::ncurl::sendemail(lparm);
			};
		}

		int64_t nguidstr2int64(const char* anguid)
		{
			if (anguid == nullptr)
			{
				return ngl::nguid::make();
			}

			ngl::ENUM_ACTOR lact_type{};
			std::string ltype_str;
			ngl::i16_area larea = nguid::none_area();
			ngl::i32_actordataid ldataid = nguid::none_actordataid();
			if (!ngl::tools::splite(anguid, "#", ltype_str, larea, ldataid))
			{
				return ngl::nguid::make();
			}
			lact_type = ngl::em<ngl::ENUM_ACTOR>::get_enum(ltype_str.c_str());
			return ngl::nguid::make(lact_type, larea, ldataid);
		}
	}
}
