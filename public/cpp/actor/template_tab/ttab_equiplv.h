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

#include "manage_csv.h"
#include "type.h"
#include "xml.h"

namespace ngl
{
	class ttab_equiplv :
		public manage_csv<tab_equiplv>
	{
		ttab_equiplv(const ttab_equiplv&) = delete;
		ttab_equiplv& operator=(const ttab_equiplv&) = delete;

		ttab_equiplv()
		{
			allcsv::loadcsv(this);
		}

		void reload()final
		{
			std::cout << "[ttab_equiplv] reload" << std::endl;
		}

	public:
		using type_tab = tab_equiplv;

		static ttab_equiplv& instance()
		{
			static ttab_equiplv ltemp;
			return ltemp;
		}

		std::map<int, tab_equiplv>& tablecsv()
		{
			ttab_equiplv* ttab = allcsv::get<ttab_equiplv>();
			if (ttab == nullptr)
			{
				tools::no_core_dump();
			}
			return ttab->m_tablecsv;
		}

		const tab_equiplv* tab(int32_t aid)
		{
			auto lmap = tablecsv();
			auto itor = lmap.find(aid);
			if (itor == lmap.end())
			{
				return nullptr;
			}
			return &itor->second;
		}
	};
}//namespace ngl
