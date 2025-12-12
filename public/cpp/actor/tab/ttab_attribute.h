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
	struct ttab_attribute :
		public manage_csv<tab_attribute>
	{
		ttab_attribute(const ttab_attribute&) = delete;
		ttab_attribute& operator=(const ttab_attribute&) = delete;

		std::map<int32_t, std::tuple<int32_t, int32_t>> m_uplowlimit;

		ttab_attribute()
		{
			allcsv::loadcsv(this);
		}

		void reload()final
		{
			std::cout << "[ttab_attribute] reload" << std::endl;
			init_uplowlimit();
		}

	public:
		using type_tab = tab_attribute;

		static ttab_attribute& instance()
		{
			static ttab_attribute ltemp;
			return ltemp;
		}

		const std::map<int, tab_attribute>* tablecsv()
		{
			ttab_attribute* ttab = allcsv::get<ttab_attribute>();
			if (ttab == nullptr)
			{
				tools::no_core_dump();
				return nullptr;
			}
			return &ttab->m_tablecsv;
		}

		const tab_attribute* tab(int32_t aid)
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
			auto ltabmap = tablecsv();
			if (ltabmap == nullptr)
			{
				tools::no_core_dump();
				return;
			}
			for (const auto& [key, value] : *ltabmap)
			{
				if (key < EnumAttribute::E_Count)
				{
					m_uplowlimit[key] = std::make_tuple(value.m_uplimit, value.m_lowlimit);
				}
			}
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

		void add(std::map<EnumAttribute, int64_t>& al, const std::map<EnumAttribute, int64_t>& ar)
		{
			for (const auto& [key, value] : ar)
			{
				al[key] += value;
			}
		}

		void add(std::map<EnumAttribute, float>& al, const std::map<EnumAttribute, float>& ar)
		{
			for (const auto& [key, value] : ar)
			{
				al[key] += value;
			}
		}

		void dec(std::map<EnumAttribute, int64_t>& al, const std::map<EnumAttribute, int64_t>& ar)
		{
			for (const auto& [key, value] : ar)
			{
				al[key] -= value;
			}
		}

		void dec(std::map<EnumAttribute, float>& al, const std::map<EnumAttribute, float>& ar)
		{
			for (const auto& [key, value] : ar)
			{
				al[key] -= value;
			}
		}
	};
}//namespace ngl
