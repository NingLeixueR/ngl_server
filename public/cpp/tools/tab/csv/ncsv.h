/*
* Copyright (c) [2020-2025] NingLeixueR
* 
* 项目名称：ngl_server
* 项目地址：https://github.com/NingLeixueR/ngl_server
* 
* 本文件是 ngl_server 项目的一部分，遵循 MIT 开源协议发布。
* 您可以按照协议规定自由使用、修改和分发本项目，包括商业用途，
* 但需保留原始版权和许可声明。
* 
* 许可详情参见项目根目录下的 LICENSE 文件：
* https://github.com/NingLeixueR/ngl_server/blob/main/LICENSE
*/
#pragma once

#include "operator_file.h"
#include "threadtools.h"
#include "enum2name.h"
#include "csvtable.h"
#include "nhash.h"
#include "tools.h"
#include "csv.h"

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
		// # 用于校验csv内容是否有变化
		virtual const std::string& verify()const = 0;
		// # csv名称
		virtual const char* csvname() = 0;
		// # 加载csv文件
		virtual void load() = 0;
		// # 根据csv id获取csv
		virtual void* find(int aid) = 0;
		// # 重新加载csv文件
		virtual void reload() = 0;

		// # [获取/设置] csv文件路径
		static std::string& path();
		static void set_path(const std::string& apath, const std::string& aname);
	};

	template <typename T>
	struct csv : 
		public csv_base
	{
		csv(const csv&) = delete;
		csv& operator=(const csv&) = delete;

		using type_tab = T;

		std::map<int, T>	m_csv;
		std::string			m_verify;		// 内容的md5值

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
			static std::string lcsvname = std::format("./{}/{}.csv", csv_base::path(), tools::type_name<T>());
			return lcsvname;
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
				});
			if (itor == m_csv.end())
			{
				return nullptr;
			}
			return &itor->second;
		}

		static nhashcode hash_code()
		{
			static nhashcode ltemp = nhash::code<T>();
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
			auto lp = tools::findmap(m_csv, tools::type_name<TTAB>());
			if (lp == nullptr)
			{
				tools::no_core_dump();
				return nullptr;
			}
			return (TTAB*)(lp->get());
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
			std::function<void(const std::string&)>		m_save;
			std::function<void()>						m_reload;
			std::function<void(std::string&)>			m_readfile;
		};
		static std::map<std::string, trefun> m_fun;
	public:
		template <typename T>
		static void register_csv()
		{
			trefun& ltemp = m_fun[tools::type_name<T>()];
			ltemp.m_save = [](const std::string& acsvcontent)
			{
				std::string lcsvname = csv<T>::path();
				tools::file_remove(lcsvname);
				writefile lrf(lcsvname);
				lrf.write(acsvcontent);
			};

			ltemp.m_reload = []()
			{
				csv_base* lpcsv = ncsv::get_csvbase(tools::type_name<T>());
				if (lpcsv == nullptr)
				{
					return;
				}
				lpcsv->load();
				lpcsv->reload();
			};

			ltemp.m_readfile = [](std::string& acsvcontent)
			{
				std::string lcsvname = csv<T>::path();
				readfile lfile(lcsvname);
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
		{//加载xxx.csv
			std::string lcsvname = path();
			ngl::rcsv lrcsv;
			if (lrcsv.read(lcsvname, m_verify))
			{
				lrcsv.readcsv(m_csv);
			}			
		}
	}
}// namespace ngl