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
// File overview: CSV table registry and batch-loading coordinator.

#pragma once

#include "tools/tools/tools_enum.h"
#include "tools/tools/tools_file.h"
#include "actor/tab/csvtable.h"
#include "tools/tools/tools_thread.h"
#include "tools/tab/csv/csv.h"
#include "tools/tools/tools_nhash.h"
#include "tools/tools.h"

#include <functional>
#include <iostream>
#include <map>

namespace ngl
{
	class csv_base
	{
		csv_base(const csv_base&) = delete;
		csv_base& operator=(const csv_base&) = delete;

		static std::string m_path;
	public:
		csv_base() = default;

		// Content hash used by reload endpoints to detect changes.
		virtual const std::string& verify()const = 0;

		// Logical table name, normally derived from the C++ type.
		virtual const char* csvname() = 0;

		// Loads or reloads the backing CSV file into memory.
		virtual void load() = 0;

		// Untyped access used by generic registries and reload helpers.
		virtual void* find(int aid) = 0;

		// Optional post-load hook for derived table wrappers.
		virtual void reload() = 0;

		// Shared root path used by every generated CSV table.
		static std::string& path();

		static void set_path(const std::string& apath);
	};

	template <typename T>
	struct csv : 
		public csv_base
	{
		csv(const csv&) = delete;
		csv& operator=(const csv&) = delete;

		using type_tab = T;

		std::map<int, T>	m_csv;
		std::string			m_verify;		// Content md5

	public:
		virtual void* find(int aid)
		{
			return tools::findmap(m_csv, aid);
		}

		T* tab(int aid)
		{
			return tools::findmap(m_csv, aid);
		}

		std::map<int, T>& tabs()
		{
			return m_csv;
		}

		virtual void reload()
		{}

		csv() = default;

		virtual const std::string& verify()const
		{
			return m_verify;
		}

		virtual const char* csvname()
		{
			return tools::type_name<T>().c_str();
		}

		static std::string path()
		{
			return std::format("./{}/{}.csv", csv_base::path(), tools::type_name<T>());
		}

		virtual void load();

		void foreach(const std::function<void(T&)>& afun)
		{
			std::ranges::for_each(m_csv, [&afun](std::pair<const int, T>& apair)
				{
					afun(apair.second);
				}
			);
		}

		T* find_if(const std::function<bool(T&)>& afun)
		{
			auto itor = std::ranges::find_if(m_csv, [&afun](std::pair<const int, T>& apair)
				{
					return afun(apair.second);
				}
			);
			if (itor == m_csv.end())
			{
				return nullptr;
			}
			return &itor->second;
		}

		static tools::nhashcode hash_code()
		{
			// Stable per-type hash used by reload/protocol metadata.
			static tools::nhashcode ltemp = tools::nhash::code<T>();
			return ltemp;
		}

		static const char* name()
		{
			return tools::type_name<T>().c_str();
		}
	};

	class ncsv
	{
		static std::map<std::string, std::shared_ptr<csv_base>> m_csv; // key: TAB::name()
	public:
		// Central table registry used by generated accessors and reload endpoints.
		static void add(const char* akey, std::shared_ptr<csv_base>& ap);

		static csv_base* get_csvbase(const std::string& akey);

		template <typename TTAB>
		static void loadcsv()
		{
			std::shared_ptr<csv_base> ltemp = std::make_shared<TTAB>();
			add(tools::type_name<TTAB>().c_str(), ltemp);
			ltemp->load();
			ltemp->reload();
		}

		template <typename TTAB>
		static TTAB* get()
		{
			csv_base* lcsv = get_csvbase(tools::type_name<TTAB>());
			if (lcsv == nullptr)
			{
				return nullptr;
			}
			return static_cast<TTAB*>(lcsv);
		}

		template <typename TAB>
		static TAB* tab(const int32_t aid)
		{
			csv<TAB>* lp = get<csv<TAB>>();
			if (lp == nullptr)
			{
				return nullptr;
			}
			void* lptab = lp->tab(aid);
			if (lptab == nullptr)
			{
				return nullptr;
			}
			return (TAB*)(lptab);
		}

		static void foreach_verify(std::map<std::string, std::string>& averify);

		static std::map<std::string, std::shared_ptr<csv_base>>& all();
	};

	class reload_csv
	{
		struct trefun
		{
			// Save/read/reload hooks are registered per table type so the hot
			// reload RPC layer does not need template knowledge.
			std::function<void(const std::string&)>		m_save;
			std::function<void()>						m_reload;
			std::function<void(std::string&)>			m_readfile;
		};
		static std::map<std::string, trefun> m_fun;
	public:
		template <typename T>
		static void register_csv()
		{
			trefun& lfun = m_fun[tools::type_name<T>()];
			lfun.m_save = [](const std::string& acsvcontent)
			{
				std::string lcsvname = csv<T>::path();
				tools::file_remove(lcsvname);
				tools::writefile lrf(lcsvname);
				lrf.write(acsvcontent);
			};

			lfun.m_reload = []()
			{
				csv_base* lpcsv = ncsv::get_csvbase(tools::type_name<T>());
				if (lpcsv == nullptr)
				{
					return;
				}
				lpcsv->load();
				lpcsv->reload();
			};

			lfun.m_readfile = [](std::string& acsvcontent)
			{
				std::string lcsvname = csv<T>::path();
				tools::readfile lfile(lcsvname);
				lfile.read(acsvcontent);
			};
		}

		static bool save(const std::string& atabhash, const std::string& acsvcontent);
		static bool reload(const std::string& atabhash);
		static bool readcsv(const std::string& atabhash, std::string& acsvcontent);
	};

	template <typename T>
	void csv<T>::load()
	{
		reload_csv::register_csv<T>();
		m_csv.clear();
		{// Loadxxx.csv
			std::string lcsvname = path();
			ngl::rcsv lrcsv;
			if (lrcsv.read(lcsvname, m_verify))
			{
				if (!lrcsv.readcsv(m_csv))
				{
					m_csv.clear();
					m_verify.clear();
				}
			}			
		}
	}
}// namespace ngl
