#pragma once

#include "manage_csv.h"
#include "nlog.h"

namespace ngl
{
	struct ttab_random : public manage_csv<tab_random>
	{
		ttab_random(const ttab_random&) = delete;
		ttab_random& operator=(const ttab_random&) = delete;

		ttab_random()
		{}

		bool is_loop(int32_t aid, std::set<int32_t>& aset)
		{
			tab_random* tab = find(aid);
			if (tab == nullptr)
			{
				return false;
			}
			if (aset.insert(tab->m_id).second == false)
			{
				return false;
			}
			for (int32_t id : tab->m_childrandomids)
			{
				if (is_loop(id, aset) == false)
				{
					return false;
				}
			}
			return true;
		}

		void reload()final;
	};
}//namespace ngl