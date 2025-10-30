/*
* Copyright (c) [2020-2025] NingLeixueR
* 
* ��Ŀ���ƣ�ngl_server
* ��Ŀ��ַ��https://github.com/NingLeixueR/ngl_server
* 
* ���ļ��� ngl_server ��Ŀ��һ���֣���ѭ MIT ��ԴЭ�鷢����
* �����԰���Э��涨����ʹ�á��޸ĺͷַ�����Ŀ��������ҵ��;��
* ���豣��ԭʼ��Ȩ�����������
* 
* �������μ���Ŀ��Ŀ¼�µ� LICENSE �ļ���
* https://github.com/NingLeixueR/ngl_server/blob/main/LICENSE
*/
#include "tprotocol.h"
#include "nscript.h"
#include "actor_manage.h"
#include "actor_base.h"

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