#pragma once

#include "manage_csv.h"

namespace ngl
{
	using map_attribute = std::map<EnumAttribute, int64_t>;
	using map_ratio = std::map<EnumAttribute, float>;
	using map_moduleratio = std::map<EnumModule, map_ratio>;
	using map_absolute = std::map<EnumAttribute, int64_t>;
	using map_attr = std::map<EnumAttribute, int64_t>;

	struct ttab_attribute : public manage_csv<tab_attribute>
	{
		ttab_attribute(const ttab_attribute&) = delete;
		ttab_attribute& operator=(const ttab_attribute&) = delete;
	private:
		static std::vector<std::pair<int32_t, int32_t>> m_uplowlimit;
	public:
		ttab_attribute()
		{}

		void reload()final
		{
			init_uplowlimit();
		}

		static tab_attribute* attr(EnumAttribute atype)
		{
			tab_attribute* tab = allcsv::tab<tab_attribute>(atype);
			return tab;
		}

		static float fightcoefficient(EnumAttribute atype)
		{
			tab_attribute* tab = allcsv::tab<tab_attribute>(atype);
			if (tab == nullptr)
				return 0.0f;
			return tab->m_fightcoefficient;
		}

		static void init_uplowlimit()
		{
			m_uplowlimit.resize(EnumAttribute::E_Count);
			auto tabcsv = allcsv::get<manage_csv<tab_attribute>>();
			for (const auto& [key, value] : tabcsv->tablecsv)
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
				init_uplowlimit();
			if (atype >= EnumAttribute::E_Count)
				return avalues;
			std::pair<int32_t, int32_t>& lpair = m_uplowlimit[atype];
			if (avalues > lpair.first)
				return lpair.first;
			else if (avalues < lpair.second)
				return lpair.second;
			else
				return avalues;
		}

		static void add(map_attribute& al, const map_attribute& ar)
		{
			for (const auto& [key, value] : ar)
				al[key] += value;
		}

		static void add(map_ratio& al, const map_ratio& ar)
		{
			for (const auto& [key, value] : ar)
				al[key] += value;
		}

		static void dec(map_attribute& al, const map_attribute& ar)
		{
			for (const auto& [key, value] : ar)
				al[key] -= value;
		}

		static void dec(map_ratio& al, const map_ratio& ar)
		{
			for (const auto& [key, value] : ar)
				al[key] -= value;
		}
	};
}// namespace ngl