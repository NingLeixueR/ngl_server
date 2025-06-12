#pragma once

#include "manage_csv.h"
#include "type.h"
#include "nlog.h"
#include "xml.h"

namespace ngl
{
	struct ttab_random :
		public manage_csv<tab_random>
	{
		ttab_random(const ttab_random&) = delete;
		ttab_random& operator=(const ttab_random&) = delete;
		using type_tab = tab_random;
		ttab_random()
		{}

		static const std::map<int, tab_random>& tablecsv()
		{
			const ttab_random* ttab = allcsv::get<ttab_random>();
			tools::core_dump(ttab == nullptr);
			return ttab->m_tablecsv;
		}
		static const tab_random* tab(int32_t aid)
		{
			const auto& lmap = tablecsv();
			auto itor = lmap.find(aid);
			if (itor == lmap.end())
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

		void reload()final;
	};
}//namespace ngl
