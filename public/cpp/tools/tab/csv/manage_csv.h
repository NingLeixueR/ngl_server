#pragma once

#include "operator_file.h"
#include "threadtools.h"
#include "enum2name.h"
#include "csvtable.h"
#include "tools.h"
#include "csv.h"

#include <functional>
#include <iostream>
#include <map>

namespace ngl
{
	class csvbase
	{
		csvbase(const csvbase&) = delete;
		csvbase& operator=(const csvbase&) = delete;
	public:
		csvbase()
		{}
		virtual const std::string&	verify()const	= 0;
		virtual const char*			csvname()		= 0;
		virtual void				load()			= 0;
		virtual void*				get(int aid)	= 0;
		virtual void				reload()		= 0;

		static std::string m_path;

		static void set_path(const std::string& apath, const std::string& aname)
		{
			m_path = std::format("{}/{}", apath, aname);
			if (tools::directories_exists(m_path) == false)
			{
				m_path = std::format("{}/csv", apath);
			}
		}

		static std::string& get_path()
		{
			return m_path;
		}
	};

	template <typename T>
	struct manage_csv : 
		public csvbase
	{
		manage_csv(const manage_csv&) = delete;
		manage_csv& operator=(const manage_csv&) = delete;

		using type_tab = T;

	protected:
		std::map<int, T>	m_tablecsv;
		std::string			m_verify;		// 内容的md5值

		virtual void* get(int aid)
		{
			auto itor = m_tablecsv.find(aid);
			if (itor == m_tablecsv.end())
			{
				return nullptr;
			}
			return &itor->second;
		}

		T* find(int aid)
		{
			auto itor = m_tablecsv.find(aid);
			if (itor == m_tablecsv.end())
			{
				return nullptr;
			}
			return &itor->second;
		}

	public:
		virtual void reload()
		{}

		manage_csv()
		{}

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
			std::string lcsvname = std::format("./{}/{}.csv", csvbase::get_path(), tools::type_name<T>());
			return lcsvname;
		}

		virtual void load();

		void foreach(const std::function<void(T&)>& afun)
		{
			for (std::pair<const int, T>& item : m_tablecsv)
			{
				afun(item.second);
			}
		}

		static int32_t hash_code()
		{
			return typeid(manage_csv<T>).hash_code();
		}

		static const char* name()
		{
			return tools::type_name<T>().c_str();
		}
	};

	class allcsv
	{
		static std::map<std::string, csvbase*> m_data; // key: TAB::name()
	public:
		static void add(const char* akey, csvbase* ap);

		static csvbase* get_csvbase(const std::string& akey);

		template <typename TTAB>
		static void loadcsv(TTAB* athis)
		{
			static std::atomic<bool> lload = true;
			if (lload.exchange(false))
			{
				using TAB = typename TTAB::type_tab;
				csvbase* lp = athis;
				allcsv::add(tools::type_name<TAB>().c_str(), lp);
				lp->load();
				lp->reload();
			}
		}

		template <typename TTAB>
		static TTAB* get()
		{
			std::string lname = TTAB::name();
			csvbase** lp = tools::findmap(m_data, lname);
			if (lp == nullptr)
			{
				tools::no_core_dump();
				return nullptr;
			}
			return (TTAB*)*lp;
		}

		template <typename TAB>
		static TAB* tab(const int32_t aid)
		{
			csvbase* lp = get<manage_csv<TAB>>();
			if (lp == nullptr)
			{
				return nullptr;
			}
			void* lptab = lp->get(aid);
			if (lptab == nullptr)
			{
				return nullptr;
			}
			return (TAB*)(lptab);
		}

		static void foreach_verify(std::map<std::string, std::string>& averify);

		static std::map<std::string, csvbase*>& all();
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
				std::string lcsvname = manage_csv<T>::path();
				tools::file_remove(lcsvname);
				writefile lrf(lcsvname);
				lrf.write(acsvcontent);
			};

			ltemp.m_reload = []()
			{
				csvbase* lpcsv = allcsv::get_csvbase(tools::type_name<T>());
				if (lpcsv == nullptr)
				{
					return;
				}
				lpcsv->load();
				lpcsv->reload();
			};

			ltemp.m_readfile = [](std::string& acsvcontent)
			{
				std::string lcsvname = manage_csv<T>::path();
				readfile lfile(lcsvname);
				lfile.read(acsvcontent);
			};
		}

		static bool save(const std::string& atabhash, const std::string& acsvcontent);
		static bool reload(const std::string& atabhash);
		static bool readcsv(const std::string& atabhash, std::string& acsvcontent);
	};

	template <typename T>
	void manage_csv<T>::load()
	{
		reload_csv::register_csv<T>();
		m_tablecsv.clear();
		{//加载xxx.csv
			std::string lcsvname = path();
			ngl::rcsv lrcsv;
			if (lrcsv.read(lcsvname, m_verify) == true)
			{
				lrcsv.readcsv(m_tablecsv);
			}			
		}
	}
}// namespace ngl
