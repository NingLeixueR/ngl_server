#pragma once

#include "manage_csv.h"
#include "type.h"
#include "xml.h"

namespace ngl
{
	class ttab_equip :
		public manage_csv<tab_equip>
	{
		ttab_equip(const ttab_equip&) = delete;
		ttab_equip& operator=(const ttab_equip&) = delete;

		ttab_equip()
		{
			allcsv::loadcsv(this);
		}

		void reload()final
		{
			std::cout << "[ttab_equip] reload" << std::endl;
		}

	public:
		using type_tab = tab_equip;

		static ttab_equip& instance()
		{
			static ttab_equip ltemp;
			return ltemp;
		}

		const std::map<int, tab_equip>& tablecsv()
		{
			const ttab_equip* ttab = allcsv::get<ttab_equip>();
			assert(ttab == nullptr);
			return ttab->m_tablecsv;
		}

		const tab_equip* tab(int32_t aid)
		{
			const auto& lmap = tablecsv();
			auto itor = lmap.find(aid);
			if (itor == lmap.end())
			{
				return nullptr;
			}
			return &itor->second;
		}
	};
}//namespace ngl
