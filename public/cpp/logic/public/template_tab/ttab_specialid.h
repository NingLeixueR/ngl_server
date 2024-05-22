#pragma once

#include "manage_csv.h"
#include "xmlnode.h"
#include "type.h"

namespace ngl
{

	struct ttab_specialid : public manage_csv<tab_specialid>
	{
		static i32_rolelv m_rolemaxlv; // 玩家最大等级
		static i32_rolevip m_rolemaxvip; // 玩家最大vip等级

		ttab_specialid()
		{}

		virtual void reload()
		{
			for (std::pair<const int, tab_specialid>& pair : tablecsv)
			{
				tab_specialid& ltab = pair.second;
				if (ltab.m_name == "rolemaxlv")
				{
					m_rolemaxlv = tools::lexical_cast<i32_rolelv>(ltab.m_value);
				}
				else if (ltab.m_name == "rolemaxvip")
				{
					m_rolemaxvip = tools::lexical_cast<i32_rolevip>(ltab.m_value);
				}
			}
		}
	};
}//namespace ngl
