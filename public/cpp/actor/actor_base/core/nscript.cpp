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

#include "actor/actor_base/core/actor_manage.h"
#include "actor/actor_base/core/actor_base.h"
#include "actor/protocol/tprotocol.h"
#include "actor/actor_base/core/nscript.h"

extern "C"
{
	int nguidstr2int64(lua_State* L)
	{
		const char* nguidstr = nullptr;
		if (ngl::nlua_stack::stack_pop(L, nguidstr))
		{
			ngl::nlua_stack::stack_push(L, true, ngl::tools::nguidstr2int64(nguidstr));
			return 2;
		}
		ngl::nlua_stack::stack_push(L, false);
		return 1;
	}

	int send_client(lua_State* L)
	{
		//actor_type#areaid#dataid
		const char* nguidstr = nullptr;
		const char* lmsgname = nullptr;
		const char* loperprotocol = nullptr;
		// "kcp" | "tcp"
		if (!ngl::nlua_stack::stack_pop(L, nguidstr, loperprotocol, lmsgname))
		{
			ngl::nlua_stack::stack_push(L, false);
			return 1;
		}
		int64_t lactorid = ngl::tools::nguidstr2int64(nguidstr);
		ngl::tprotocol::info* linfo = ngl::tprotocol::get(lmsgname);
		if (linfo == nullptr)
		{
			ngl::nlua_stack::stack_push(L, false);
			return 1;
		}
		auto lcall = linfo->toclient<ngl::enscript_lua>();
		if (lcall != nullptr && !(*lcall)(lactorid, loperprotocol, L))
		{
			ngl::nlua_stack::stack_push(L, false);
			return 1;
		}
		lua_pushboolean(L, true);
		return 1;
	}

	int send_actor(lua_State* L)
	{
		const char* nguidstr = nullptr;
		const char* lmsgname = nullptr;
		if (!ngl::nlua_stack::stack_pop(L, nguidstr, lmsgname))
		{
			ngl::nlua_stack::stack_push(L, false);
			return 1;
		}
		int64_t lactorid = ngl::tools::nguidstr2int64(nguidstr);
		ngl::tprotocol::info* linfo = ngl::tprotocol::get(lmsgname);
		if (linfo == nullptr)
		{
			ngl::nlua_stack::stack_push(L, false);
			return 1;
		}
		auto lcall = linfo->toactor<ngl::enscript_lua>();
		if (lcall != nullptr && !(*lcall)(lactorid, L))
		{
			ngl::nlua_stack::stack_push(L, false);
			return 1;
		}
		lua_pushboolean(L, true);
		return 1;
	}	
}//extern "C"
