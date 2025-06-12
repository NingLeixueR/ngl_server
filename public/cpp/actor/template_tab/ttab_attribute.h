#pragma once

#include "manage_csv.h"
#include "type.h"
#include "xml.h"

namespace ngl
{
	using map_attrvalue = std::map<EnumAttribute, int64_t>;
	using map_attrratio = std::map<EnumAttribute, float>;
	using map_moduleratio = std::map<EnumModule, map_attrratio>;

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

		const std::map<int, tab_attribute>& tablecsv()
		{
			const ttab_attribute* ttab = allcsv::get<ttab_attribute>();
			assert(ttab != nullptr);
			return ttab->m_tablecsv;
		}

		const tab_attribute* tab(int32_t aid)
		{
			const auto& lmap = tablecsv();
			auto itor = lmap.find(aid);
			if (itor == lmap.end())
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
			for (const auto& [key, value] : tablecsv())
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

		void add(map_attrvalue& al, const map_attrvalue& ar)
		{
			for (const auto& [key, value] : ar)
			{
				al[key] += value;
			}
		}

		void add(map_attrratio& al, const map_attrratio& ar)
		{
			for (const auto& [key, value] : ar)
			{
				al[key] += value;
			}
		}

		void dec(map_attrvalue& al, const map_attrvalue& ar)
		{
			for (const auto& [key, value] : ar)
			{
				al[key] -= value;
			}
		}

		void dec(map_attrratio& al, const map_attrratio& ar)
		{
			for (const auto& [key, value] : ar)
			{
				al[key] -= value;
			}
		}
	};
}//namespace ngl
