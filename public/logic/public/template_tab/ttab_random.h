#pragma once

#include "manage_csv.h"
#include "nlog.h"


namespace ngl
{
	struct ttab_random : public manage_csv<tab_random>
	{
		ttab_random()
		{}

		bool is_loop(int32_t aid, std::set<int32_t>& aset)
		{
			tab_random* tab = find(aid);
			if (tab == nullptr)
				return false;
			if (aset.insert(tab->m_id).second == false)
				return false;
			for (int32_t id : tab->m_childrandomids)
			{
				if (is_loop(id, aset) == false)
					return false;
			}
			return true;
		}


		virtual void reload()
		{
			// ## 检查所有子掉落是否循环引用
			for (std::pair<const int, tab_random>& ipair : tablecsv)
			{
				tab_random& tab = ipair.second;
				std::set<int32_t> lset;
				if (is_loop(tab.m_id, lset) == false)
				{
					LogLocalError("ttab_random::reload random[%] loop!!!", tab.m_id);
				}
			}
		}
	};
}