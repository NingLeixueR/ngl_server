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
// File overview: Declares interfaces for nmap.

#pragma once

#include "actor/actor_logic/actor_role/logic/attribute/unit.h"
#include "actor/actor_logic/actor_role/logic/nmap/aoi.h"
#include "actor/tab/csvtable.h"
#include "actor/pb/net.pb.h"
#include "tools/type.h"

#include <vector>
#include <list>
#include <map>
#include <set>

namespace ngl
{
	class grid
	{
		std::set<i64_actorid> m_unitlist;
	public:
		// Translated comment.
		bool enter(i64_actorid aunitid);

		// # Leave
		void leave(i64_actorid aunitid);

		// # Getunitlist
		const std::set<i64_actorid>& get_unitlist();

		void clear();

		int32_t count();
	};
}//namespace ngl