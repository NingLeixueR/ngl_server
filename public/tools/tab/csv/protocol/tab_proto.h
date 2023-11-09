#pragma once

#include <map>
#include <iostream>
#include <functional>

#include "operator_file.h"
#include "regular.h"
#include "tools.h"

namespace ngl
{
	template <typename T>
	struct tab_proto
	{
		using TAB = T;

		static T table;
		
		static const char* csvname()
		{
			return T().descriptor()->full_name().c_str();
		}

		static bool path(std::string& apath)
		{
			const char* dir = "./csv/protocol/";
			apath = dir;
			apath += csvname();
			apath += ".data";
			return true;
		}

		static void load()
		{
			std::string lcsvname;
			if (path(lcsvname) == false)
			{
				std::cout << "csv-protocol error [" << lcsvname << "]" << std::endl;
				return;
			}
			regular::replace("Template.", "", lcsvname, lcsvname);

			readfile lrf(lcsvname);
			std::pair<char*, int> lpair = lrf.readcurrent();
			if (table.ParseFromArray(lpair.first, lpair.second) == false)
				return;
			delete []lpair.first;
		}

		static void foreach(const std::function<void(typename T::data&)>& afun)
		{
			for (auto itor = table.m_data.begin(); itor != table.m_data.end(); ++itor)
			{
				afun(itor->second);
			}				
		}

		static int32_t hash_code()
		{
			return typeid(tab_proto<T>).hash_code();
		}

		static T::data* find(int32_t aid)
		{
			auto itor = table.mutable_m_data()->find(aid);
			if (itor == table.mutable_m_data()->end())
				return nullptr;
			return &itor->second;
		}
	};

	template <typename T>
	T tab_proto<T>::table;


	struct tab_proto_loadall
	{
		static void loadall()
		{
			/*
			tab_proto<pbdb::tab_errormessage>::load();
			*/
		}
	};

}