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
	public:
		virtual const std::string&	verify()const	= 0;
		virtual const char*			csvname()		= 0;
		virtual void				load()			= 0;
		virtual void*				get(int aid)	= 0;
		virtual void				reload()		= 0;
	};

	template <typename T>
	struct manage_csv : public csvbase
	{
		using TAB = T;

		std::map<int, T>	tablecsv;
		std::string			m_verify;		// 内容的md5值
		
		virtual const std::string& verify()const
		{
			return m_verify;
		}

		virtual const char* csvname()
		{
			return T::name();
		}

		static std::string path()
		{
			const char* dir = "./csv/";
			std::string lcsvname(dir);
			lcsvname += T::name();
			lcsvname += ".csv";
			return std::move(lcsvname);
		}

		virtual void load();

		virtual void* get(int aid)
		{
			auto itor = tablecsv.find(aid);
			if (itor == tablecsv.end())
			{
				return nullptr;
			}
			else
			{
				return &itor->second;
			}
		}

		T* find(int aid)
		{
			auto itor = tablecsv.find(aid);
			if (itor == tablecsv.end())
			{
				return nullptr;
			}
			else
			{
				return &itor->second;
			}
		}

		virtual void reload()
		{}

		void foreach(const std::function<void(T&)>& afun)
		{
			for (std::pair<const int, T>& item : tablecsv)
				afun(item.second);
		}

		static int32_t hash_code()
		{
			return typeid(manage_csv<T>).hash_code();
		}

		static const char* name()
		{
			return T::name();
		}
	};

	class allcsv
	{
		static std::map<std::string, csvbase*> m_data; // key: TAB::name()
	public:
		static void load();

		static void add(const char* akey, csvbase* ap);

		static csvbase* get_csvbase(const std::string& akey);

		template <typename TAB>
		static TAB* get()
		{
			std::string lname = TAB::name();
			csvbase** lp = tools::findmap(m_data, lname);
			return lp == nullptr ? nullptr : (TAB*)*lp;
		}

		template <typename TAB>
		static TAB* tab(const int32_t aid)
		{
			csvbase* lp = get<manage_csv<TAB>>();
			if (lp == nullptr)
				return nullptr;
			void* lptab = lp->get(aid);
			if (lptab == nullptr)
				return nullptr;
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
			trefun& ltemp = m_fun[T::name()];
			ltemp.m_save = [](const std::string& acsvcontent)
			{
				std::string lcsvname = manage_csv<T>::path();
				filetools::remove(lcsvname);
				writefile lrf(lcsvname);
				lrf.write(acsvcontent);
			};

			ltemp.m_reload = []()
			{
				csvbase* lpcsv = allcsv::get_csvbase(T::name());
				if (lpcsv == nullptr)
					return;
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
		tablecsv.clear();
		{//加载xxx.csv
			std::string lcsvname = path();
			ngl::rcsv lrcsv;
			if (lrcsv.read(lcsvname, m_verify) == true)
			{
				lrcsv.readcsv(tablecsv);
			}			
		}
	}
}// namespace ngl
