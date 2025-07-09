#include "tprotocol.h"
#include "nscript.h"

extern "C"
{
	int send_client(lua_State* L)
	{
		//actor_type#areaid#dataid
		const char* nguidstr = luaL_checkstring(L, 1);
		ngl::ENUM_ACTOR lactortype;
		std::string lactortypestr;
		ngl::i16_area larea;
		ngl::i32_actordataid ldataid;
		ngl::tools::splite(nguidstr, "#", lactortypestr, larea, ldataid);
		lactortype = ngl::em<ngl::ENUM_ACTOR>::get_enum(lactortypestr.c_str());
		ngl::i64_actorid lactorid = ngl::nguid::make(lactortype, larea, ldataid);

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
		ngl::i64_actorid lactorid = ngl::tools::lexical_cast<ngl::i64_actorid>(nguidstr);

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
	std::map<enscript, std::shared_ptr<nscript>> nscript::m_data;
}//namespace ngl