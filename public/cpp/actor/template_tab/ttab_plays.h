#pragma once

#include "manage_csv.h"
#include "type.h"
#include "xml.h"

namespace ngl
{
	class ttab_plays :
		public manage_csv<tab_plays>
	{
		ttab_plays(const ttab_plays&) = delete;
		ttab_plays& operator=(const ttab_plays&) = delete;

		ttab_plays()
		{
			allcsv::loadcsv(this);
		}

		void reload()final
		{
			std::cout << "[ttab_plays] reload" << std::endl;
		}

	public:
		using type_tab = tab_plays;

		static ttab_plays& instance()
		{
			static ttab_plays ltemp;
			return ltemp;
		}

		const std::map<int, tab_plays>& tablecsv()
		{
			const ttab_plays* ttab = allcsv::get<ttab_plays>();
			tools::core_dump(ttab == nullptr);
			return ttab->m_tablecsv;
		}

		const tab_plays* tab(int32_t aid)
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
