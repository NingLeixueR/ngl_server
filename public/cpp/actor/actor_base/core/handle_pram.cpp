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
// File overview: Implements handle_pram construction and protocol-type resolution.

#include "actor/actor_base/core/handle_pram.h"
#include "actor/actor_base/core/naddress.h"
#include "actor/actor_base/core/nguid.h"

namespace ngl
{
	/**
	 * Constructor for node-session pair with explicit session ID.
	 * @param asession The session ID
	 * @param anode The actor node information
	 */
	nnode_session::nnode_session(i32_sessionid asession, const nactornode& anode) :
		m_session(asession),
		m_node(anode)
	{}

	/**
	 * Constructor for node-session pair with default session ID.
	 * @param anode The actor node information
	 */
	nnode_session::nnode_session(const nactornode& anode) :
		m_node(anode)
	{}

	/**
	 * Extract the server ID from an actor ID.
	 * @param aactorid The actor ID to parse
	 * @return The server ID component
	 */
	i32_serverid handle_pram::serverid(i64_actorid aactorid)
	{
		return naddress::serverid(aactorid);
	}

	/**
	 * Extract the gateway ID from an actor ID.
	 * @param aactorid The actor ID to parse
	 * @return The gateway ID component
	 */
	i32_serverid handle_pram::gatewayid(i64_actorid aactorid)
	{
		return naddress::gatewayid(aactorid);
	}

	/**
	 * Get a list of all server IDs for a specific actor type.
	 * @param atype The actor type to query
	 * @param avec Output set to store the server IDs
	 */
	void handle_pram::serveridlist(ENUM_ACTOR atype, std::set<i32_serverid>& avec)
	{
		naddress::serveridlist(atype, avec);
	}

	/**
	 * Check if a GUID represents a null/invalid actor ID.
	 * @param aguid The GUID to check
	 * @return True if the actor ID is null/invalid, false otherwise
	 */
	bool handle_pram::is_actoridnone(const nguid& aguid)
	{
		return aguid.is_actoridnone();
	}
}//namespace ngl