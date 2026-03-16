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
// File overview: Implements logic for actor base.

#include "actor/tab/ttab_servers.h"
#include "actor/actor_base/nguid.h"

namespace ngl
{
	i64_actorid nguid::make_self(ENUM_ACTOR atype)
	{
		nguid lguid(atype, tab_self_area, none_actordataid());
		return (i64_actorid)lguid;
	}
}//namespace ngl