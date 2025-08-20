#include "tprotocol.h"
#include "nscript.h"
#include "actor_manage.h"
#include "actor_base.h"

extern "C"
{
	int nguidstr2int64(lua_State* L)
	{
		const char* nguidstr = luaL_checkstring(L, 1);
		ngl::nlua_stack::stack_push(L, ngl::tools::nguidstr2int64(nguidstr));
		return 1;
	}

	int send_client(lua_State* L)
	{
		//actor_type#areaid#dataid
		const char* nguidstr = luaL_checkstring(L, 1);
		int64_t lactorid = ngl::tools::nguidstr2int64(nguidstr);

		const char* lmsgname = luaL_checkstring(L, 2);
		const char* ljsonmsg = luaL_checkstring(L, 3);
		ngl::tprotocol::pinfo* linfo = ngl::tprotocol::getbyname(lmsgname);
		if (linfo == nullptr)
		{
			lua_pushboolean(L, false);
			return 1;
		}
		linfo->m_toclient(lactorid, ljsonmsg);

		lua_pushboolean(L, true);
		return 1;
	}

	int send_actor(lua_State* L)
	{
		const char* nguidstr = luaL_checkstring(L, 1);
		ngl::i64_actorid lactorid = ngl::tools::nguidstr2int64(nguidstr);

		const char* lmsgname = luaL_checkstring(L, 2);
		const char* ljsonmsg = luaL_checkstring(L, 3);
		ngl::tprotocol::pinfo* linfo = ngl::tprotocol::getbyname(lmsgname);
		if (linfo == nullptr)
		{
			lua_pushboolean(L, false);
			return 1;
		}
		linfo->m_toactor(lactorid, ljsonmsg);

		lua_pushboolean(L, true);
		return 1;
	}	
}

namespace ngl
{



}//namespace ngl