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
#include "nlog.h"
#include "xml.h"
#include "nlog.h"

namespace ngl
{
	class ttab_kcp :
		public manage_csv<tab_kcp>
	{
		ttab_kcp(const ttab_kcp&) = delete;
		ttab_kcp& operator=(const ttab_kcp&) = delete;

		ttab_kcp()
		{
			allcsv::loadcsv(this);
		}

		void reload()final
		{
			std::cout << "[ttab_kcp] reload" << std::endl;
		}

	public:
		using type_tab = tab_kcp;

		static ttab_kcp& instance()
		{
			static ttab_kcp ltemp;
			return ltemp;
		}

		const std::map<int, tab_kcp>* tablecsv()
		{
			ttab_kcp* ttab = allcsv::get<ttab_kcp>();
			if (ttab == nullptr)
			{
				tools::no_core_dump();
				return nullptr;
			}
			return &ttab->m_tablecsv;
		}

		const tab_kcp* tab(int32_t aid)
		{
			auto lpmap = tablecsv();
			if (lpmap == nullptr)
			{
				return nullptr;
			}
			auto itor = lpmap->find(aid);
			if (itor == lpmap->end())
			{
				return nullptr;
			}
			return &itor->second;
		}
	};
}//namespace ngl
