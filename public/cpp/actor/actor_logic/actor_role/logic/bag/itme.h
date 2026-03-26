/*
* Copyright (c) [2020-2025] NingLeixueR
* 
* Project name: ngl_server
* Project URL: https://github.com/NingLeixueR/ngl_server
* 
* This file is part of the ngl_server project and is distributed under the MIT License.
* You may use, modify, and distribute this project under the license, including commercial use,
* but you must retain the original copyright and license notice.
* 
* For license details, see the LICENSE file in the project root:
* https://github.com/NingLeixueR/ngl_server/blob/main/LICENSE
*/
// File overview: Declares interfaces for bag.

#pragma once

#include "actor/tab/ttab_item.h"
#include "tools/tab/csv/ncsv.h"
#include "actor/generated/pb/db.pb.h"

namespace ngl
{
	class actor_role;
	
	class item_create
	{
		static std::map<EItemType, item_create*> m_data;
	protected:
		explicit item_create(EItemType atype)
		{
			m_data[atype] = this;
		}

		virtual bool mode(actor_role* arole, int32_t atid, int32_t acount, std::vector<pbdb::item>& avec) = 0;
	public:
		static bool create(actor_role* arole, int32_t atid, int32_t acount, std::vector<pbdb::item>& avec)
		{
			auto tab = ttab_item::instance().tab(atid);
			if (tab == nullptr)
			{
				return false;
			}
			auto itor = m_data.find((EItemType)tab->m_type);
			if (itor == m_data.end())
			{
				return false;
			}
			return itor->second->mode(arole, atid, acount, avec);
		}
	};
}// namespace ngl