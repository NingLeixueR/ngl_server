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

#include "tools/tab/csv/ncsv.h"
#include "tools/tab/xml/xml.h"
#include "tools/type.h"

namespace ngl
{
	struct ttab_attribute :
		public csv<tab_attribute>
	{
		ttab_attribute(const ttab_attribute&) = delete;
		ttab_attribute& operator=(const ttab_attribute&) = delete;

		std::map<int32_t, std::tuple<int32_t, int32_t>> m_uplowlimit;

		void reload()final
		{
			std::cout << "[ttab_attribute] reload" << std::endl;
		}
	public:
		using type_tab = tab_attribute;

		ttab_attribute() = default;

		static ttab_attribute& instance()
		{
			static std::atomic lload = true;
			if (lload.exchange(false))
			{
				ncsv::loadcsv<ttab_attribute>();
			}
			return *ncsv::get<ttab_attribute>();
		}

		// # std::map<int, tab_attribute>& tabs()
		// # tab_attribute* tab(int aid)

		const tab_attribute* attr(EnumAttribute atype)
		{
			return tab(atype);
		}

		float fightcoefficient(EnumAttribute atype)
		{
			const tab_attribute* ltab = tab(atype);
			if (ltab == nullptr)
			{
				return 0.0f;
			}
			return ltab->m_fightcoefficient;
		}

		void init_uplowlimit()
		{
			foreach([&](tab_attribute& atab)
				{
					if (atab.m_id < EnumAttribute::E_Count)
					{
						m_uplowlimit[atab.m_id] = std::make_tuple(atab.m_uplimit, atab.m_lowlimit);
					}
				}
			);
		}

		int32_t uplowlimit(EnumAttribute atype, int32_t avalues)
		{
			if (m_uplowlimit.empty())
			{
				init_uplowlimit();
			}
			if (atype >= EnumAttribute::E_Count)
			{
				return avalues;
			}
			std::tuple<int32_t, int32_t>& lpair = m_uplowlimit[atype];
			if (avalues > std::get<0>(lpair))
			{
				return std::get<0>(lpair);
			}
			else if (avalues < std::get<1>(lpair))
			{
				return std::get<1>(lpair);
			}
			else
			{
				return avalues;
			}
		}
	};
}//namespace ngl