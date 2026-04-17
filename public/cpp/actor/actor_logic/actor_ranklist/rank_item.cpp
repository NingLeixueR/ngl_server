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
// File overview: Implements rank entry scoring and comparison operations.


#include "actor/actor_logic/actor_ranklist/actor_ranklist.h"
#include "actor/actor_logic/actor_ranklist/rank_item.h"

namespace ngl
{
	const pbdb::db_brief* rank_item::get()const
	{
		return tdb_brief::nsp_cwrite<actor_role>::instance(actor_ranklist::actorid()).getconst(m_actorid);
	}
}//namespace ngl