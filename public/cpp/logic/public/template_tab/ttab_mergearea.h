#pragma once

#include "manage_csv.h"
#include "tools.h"
#include "nguid.h"
#include "type.h"
#include "nlog.h"
#include "xml.h"

namespace ngl
{
	struct ttab_mergearea : public manage_csv<tab_mergearea>
	{
		ttab_mergearea(const ttab_mergearea&) = delete;
		ttab_mergearea& operator=(const ttab_mergearea&) = delete;

		// key: ����id value: �ϲ����ĸ�����
		static std::map<i16_area, i16_area> m_merge1;
		// key: �Ϸ�����id value: ��Щ�����ڴ�����
		static std::map<i16_area, std::set<i16_area>> m_merge2;

		ttab_mergearea()
		{}

		void reload()final
		{
			for (std::pair<const int, tab_mergearea>& pair : m_tablecsv)
			{
				i16_area larea = (i16_area)pair.second.m_id;
				i16_area lmergeid = (i16_area)pair.second.m_mergeid;

				m_merge1[larea] = lmergeid;
				m_merge2[lmergeid].insert(larea);
			}
		}

		// ��Щ�����ڴ�����
		static std::set<i16_area>* mergelist(i16_area aarea)
		{
			return tools::findmap(m_merge2, aarea);
		}

		static i16_area mergeid(i16_area aarea)
		{
			i16_area* ret = tools::findmap(m_merge1, aarea);
			if (ret == nullptr)
			{
				return nguid::none_area();
			}
			return *ret;
		}
	};
}//namespace ngl
