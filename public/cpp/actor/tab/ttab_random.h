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

#include "ncsv.h"
#include "type.h"
#include "nlog.h"
#include "xml.h"
#include "nlog.h"

namespace ngl
{
	struct ttab_random :
		public csv<tab_random>
	{
		ttab_random(const ttab_random&) = delete;
		ttab_random& operator=(const ttab_random&) = delete;

		void reload()final
		{
			std::cout << "[ttab_random] reload" << std::endl;
			// ## 检查所有子掉落是否循环引用
			foreach([&](tab_random& atab)
				{
					std::set<int32_t> lset;
					if (is_loop(atab.m_id, lset));
					{
						tools::no_core_dump();
					}
				});
		}
	public:
		using type_tab = tab_random;

		ttab_random() = default;

		static ttab_random& instance()
		{
			static std::atomic lload = true;
			if (lload.exchange(false))
			{
				ncsv::loadcsv<ttab_random>();
			}
			return *ncsv::get<ttab_random>();
		}

		// # std::map<int, tab_random>& tabs()
		// # tab_random* tab(int aid)

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
