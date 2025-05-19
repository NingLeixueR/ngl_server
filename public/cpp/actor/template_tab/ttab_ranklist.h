#pragma once

#include "manage_csv.h"
#include "type.h"
#include "xml.h"

namespace ngl
{

	struct ttab_ranklist : public manage_csv<tab_ranklist>
	{
		ttab_ranklist(const ttab_ranklist&) = delete;
		ttab_ranklist& operator=(const ttab_ranklist&) = delete;

		ttab_ranklist()
		{}

		static const tab_ranklist* tab(pbdb::eranklist atype)
		{
			const ttab_ranklist* ttab = allcsv::get<ttab_ranklist>();
			if (ttab == nullptr)
			{
				return nullptr;
			}
			auto itor = ttab->m_tablecsv.find(atype);
			return &itor->second;
		}
	};
}//namespace ngl