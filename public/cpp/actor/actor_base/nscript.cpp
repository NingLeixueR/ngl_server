#include "tprotocol.h"
#include "nscript.h"

extern "C"
{
	int send_client(lua_State* L)
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
		linfo->m_toclient(lactorid, ljsonmsg);

		lua_pushboolean(L, true);
		return 1;
	}
}

namespace ngl
{
	std::map<enscript, std::shared_ptr<nscript>> nscript::m_data;
}//namespace ngl