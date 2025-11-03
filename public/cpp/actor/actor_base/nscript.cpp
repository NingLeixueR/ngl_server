/*
* Copyright (c) [2020-2025] NingLeixueR
* 
* 项目名称：ngl_server
* 项目地址：https://github.com/NingLeixueR/ngl_server
* 
* 本文件是 ngl_server 项目的一部分，遵循 MIT 开源协议发布。
* 您可以按照协议规定自由使用、修改和分发本项目，包括商业用途，
* 但需保留原始版权和许可声明。
* 
* 许可详情参见项目根目录下的 LICENSE 文件：
* https://github.com/NingLeixueR/ngl_server/blob/main/LICENSE
*/
#include "actor_manage.h"
#include "actor_base.h"
#include "tprotocol.h"
#include "nscript.h"

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
		if (!ngl::nlua_stack::stack_pop(L, nguidstr, lmsgname))
		{
			ngl::nlua_stack::stack_push(L, false);
			return 1;
		}
		int64_t lactorid = ngl::tools::nguidstr2int64(nguidstr);
		ngl::tprotocol::info* linfo = ngl::tprotocol::getbyname(lmsgname);
		if (linfo == nullptr)
		{
			ngl::nlua_stack::stack_push(L, false);
			return 1;
		}
		auto lcall = linfo->toclient<ngl::enscript_lua>();
		if (lcall != nullptr && !lcall(lactorid, L))
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
		ngl::tprotocol::info* linfo = ngl::tprotocol::getbyname(lmsgname);
		if (linfo == nullptr)
		{
			ngl::nlua_stack::stack_push(L, false);
			return 1;
		}
		auto lcall = linfo->toactor<ngl::enscript_lua>();
		if (lcall != nullptr && !lcall(lactorid, L))
		{
			ngl::nlua_stack::stack_push(L, false);
			return 1;
		}
		lua_pushboolean(L, true);
		return 1;
	}	
}//extern "C"