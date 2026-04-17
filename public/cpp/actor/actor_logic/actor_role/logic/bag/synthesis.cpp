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
// File overview: Implements item synthesis and crafting recipes.


#include "actor/actor_logic/actor_role/logic/bag/synthesis.h"
#include "actor/actor_logic/actor_role/logic/bag/consume.h"
#include "actor/actor_logic/actor_role/logic/bag/drop.h"
#include "actor/actor_logic/actor_role/actor_role.h"
#include "actor/tab/ttab_synthesis.h"

namespace ngl
{
	bool synthesis::use(actor_role* arole, int32_t aid)
	{
		tab_synthesis* tab = ttab_synthesis::instance().tab(aid);
		if (tab == nullptr)
		{
			return false;
		}
		if (consume::use(arole, tab->m_consume, 1) == false)
		{
			log_error()->print("synthesis::use id:{} role:{} consume::use faile {}", aid, arole->id_guid(), tab->m_consume);
			return false;
		}
		std::string lsrc = std::format("synthesis::use({},{})", nguid(arole->id_guid()), aid);
		if (arole->get_drop().use(tab->m_generate, 1, arole->id_guid(), lsrc, nullptr) == false)
		{
			log_error()->print("synthesis::use id:{} role:{} drop::use faile {}", aid, arole->id_guid(), tab->m_generate);
			return false;
		}
		return true;
	}
}// namespace ngl