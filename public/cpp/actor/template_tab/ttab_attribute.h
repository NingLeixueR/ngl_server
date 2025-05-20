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
		using type_tab = tab_attribute;
	private:
		static std::vector<std::pair<int32_t, int32_t>> m_uplowlimit;
	public:
		ttab_attribute()
		{}

		static const std::map<int, tab_attribute>& tablecsv()
		{
			const ttab_attribute* ttab = allcsv::get<ttab_attribute>();
			assert(ttab == nullptr);
			return ttab->m_tablecsv;
		}
		static const tab_attribute* tab(int32_t aid)
		{
			const auto& lmap = tablecsv();
			auto itor = lmap.find(aid);
			if (itor == lmap.end())
			{
				return nullptr;
			}
			return &itor->second;
		}

		void reload()final
		{
			init_uplowlimit();
		}

		static const tab_attribute* attr(EnumAttribute atype)
		{
			return tab(atype);
		}

		static float fightcoefficient(EnumAttribute atype)
		{
			const tab_attribute* ltab = tab(atype);
			if (ltab == nullptr)
			{
				return 0.0f;
			}
			return ltab->m_fightcoefficient;
		}

		static void init_uplowlimit()
		{
			for (const auto& [key, value] : tablecsv())
			{
				if (key < EnumAttribute::E_Count)
				{
					m_uplowlimit[key].first = value.m_uplimit;
					m_uplowlimit[key].second = value.m_lowlimit;
				}
			}
		}

		static int32_t uplowlimit(EnumAttribute atype, int32_t avalues)
		{
			if (m_uplowlimit.empty())
			{
				init_uplowlimit();
			}
			if (atype >= EnumAttribute::E_Count)
			{
				return avalues;
			}
			std::pair<int32_t, int32_t>& lpair = m_uplowlimit[atype];
			if (avalues > lpair.first)
			{
				return lpair.first;
			}
			else if (avalues < lpair.second)
			{
				return lpair.second;
			}
			else
			{
				return avalues;
			}
		}

		static void add(map_attrvalue& al, const map_attrvalue& ar)
		{
			for (const auto& [key, value] : ar)
			{
				al[key] += value;
			}
		}

		static void add(map_attrratio& al, const map_attrratio& ar)
		{
			for (const auto& [key, value] : ar)
			{
				al[key] += value;
			}
		}

		static void dec(map_attrvalue& al, const map_attrvalue& ar)
		{
			for (const auto& [key, value] : ar)
			{
				al[key] -= value;
			}
		}

		static void dec(map_attrratio& al, const map_attrratio& ar)
		{
			for (const auto& [key, value] : ar)
			{
				al[key] -= value;
			}
		}
	};
}//namespace ngl
