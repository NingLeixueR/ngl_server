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

#include "actor/actor_base/core/handle_pram.h"
#include "actor/actor_base/core/naddress.h"
#include "actor/actor_base/core/nguid.h"

namespace ngl
{
	nnode_session::nnode_session(i32_sessionid asession, const nactornode& anode) :
		m_session(asession),
		m_node(anode)
	{}

	nnode_session::nnode_session(const nactornode& anode) :
		m_node(anode)
	{}

	i32_serverid handle_pram::serverid(i64_actorid aactorid)
	{
		return naddress::serverid(aactorid);
	}

	i32_serverid handle_pram::gatewayid(i64_actorid aactorid)
	{
		return naddress::gatewayid(aactorid);
	}

	void handle_pram::serveridlist(ENUM_ACTOR atype, std::set<i32_serverid>& avec)
	{
		naddress::serveridlist(atype, avec);
	}

	bool handle_pram::is_actoridnone(const nguid& aguid)
	{
		return aguid.is_actoridnone();
	}
}//namespace ngl