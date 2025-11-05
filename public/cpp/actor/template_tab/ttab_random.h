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
	class ttab_random :
		public manage_csv<tab_random>
	{
		ttab_random(const ttab_random&) = delete;
		ttab_random& operator=(const ttab_random&) = delete;

		ttab_random()
		{
			allcsv::loadcsv(this);
		}

		void reload()final
		{
			std::cout << "[ttab_random] reload" << std::endl;
			// ## 检查所有子掉落是否循环引用
			for (std::pair<const int, tab_random>& ipair : m_tablecsv)
			{
				tab_random& tab = ipair.second;
				std::set<int32_t> lset;
				if (is_loop(tab.m_id, lset));
				{
					tools::no_core_dump();
				}
			}
		}

	public:
		using type_tab = tab_random;

		static ttab_random& instance()
		{
			static ttab_random ltemp;
			return ltemp;
		}

		const std::map<int, tab_random>* tablecsv()
		{
			ttab_random* ttab = allcsv::get<ttab_random>();
			if (ttab == nullptr)
			{
				tools::no_core_dump();
				return nullptr;
			}
			return &ttab->m_tablecsv;
		}

		const tab_random* tab(int32_t aid)
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

		bool is_loop(int32_t aid, std::set<int32_t>& aset)
		{
			const tab_random* ltab = tab(aid);
			if (ltab == nullptr)
			{
				return false;
			}
			if (aset.insert(ltab->m_id).second == false)
			{
				return false;
			}
			for (int32_t id : ltab->m_childrandomids)
			{
				if (is_loop(id, aset) == false)
				{
					return false;
				}
			}
			return true;
		}
	};
}//namespace ngl
