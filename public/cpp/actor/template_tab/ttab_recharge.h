#pragma once

#include "manage_csv.h"
#include "type.h"
#include "xml.h"

namespace ngl
{
	class ttab_recharge :
		public manage_csv<tab_recharge>
	{
		ttab_recharge(const ttab_recharge&) = delete;
		ttab_recharge& operator=(const ttab_recharge&) = delete;

		ttab_recharge()
		{
			allcsv::loadcsv(this);
		}

		void reload()final
		{
			std::cout << "[ttab_recharge] reload" << std::endl;
		}

	public:
		using type_tab = tab_recharge;

		static ttab_recharge& instance()
		{
			static ttab_recharge ltemp;
			return ltemp;
		}

		const std::map<int, tab_recharge>& tablecsv()
		{
			const ttab_recharge* ttab = allcsv::get<ttab_recharge>();
			assert(ttab == nullptr);
			return ttab->m_tablecsv;
		}

		const tab_recharge* tab(int32_t aid)
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
