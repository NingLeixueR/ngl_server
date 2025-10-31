#include "serialize_lua.h"

namespace ngl
{
	bool luaapi::isnumber(lua_State* L)
	{
		return lua_isnumber(L, -1);
	}

	void luaapi::pushnumber(lua_State* L, double adata)
	{
		lua_pushnumber(L, adata);
	}

	bool luaapi::popnumber(lua_State* L, double& adata, bool apop/* = true*/)
	{
		if (!isnumber(L))
		{
			return false;
		}
		adata = lua_tonumber(L, -1);
		if (apop)
		{
			lua_pop(L, 1);
		}
		return true;
	}

	bool luaapi::isnil(lua_State* L)
	{
		return lua_isnil(L, -1);
	}

	void luaapi::pushnil(lua_State* L)
	{
		lua_pushnil(L);
	}

	bool luaapi::popnil(lua_State* L, bool apop/* = true*/)
	{
		if (!isnil(L))
		{
			return false;
		}
		if (apop)
		{
			lua_pop(L, 1);
		}
		return true;
	}

	bool luaapi::isinteger(lua_State* L)
	{
		return lua_isinteger(L, -1);
	}

	void luaapi::pushinteger(lua_State* L, int64_t aval)
	{
		lua_pushinteger(L, aval);
	}

	bool luaapi::popinteger(lua_State* L, int64_t& aval, bool apop/* = true*/)
	{
		if (!isinteger(L))
		{
			return false;
		}
		aval = lua_tointeger(L, -1);
		if (apop)
		{
			lua_pop(L, 1);
		}
		return true;
	}

	bool luaapi::isstring(lua_State* L)
	{
		return lua_isstring(L, -1);
	}

	const char* luaapi::pushstring(lua_State* L, const char* astr)
	{
		return lua_pushstring(L, astr);
	}

	const char* luaapi::pushstring(lua_State* L, const std::string& astr)
	{
		return lua_pushstring(L, astr.c_str());
	}

	bool luaapi::popstring(lua_State* L, std::string& aval, bool apop/* = true*/)
	{
		if (!isstring(L))
		{
			return false;
		}
		const char* lpstr = lua_tostring(L, -1);
		if (lpstr == nullptr)
		{
			return false;
		}
		aval = lpstr;
		if (apop)
		{
			lua_pop(L, 1);
		}
		return true;
	}
	
	bool luaapi::popstring(lua_State* L, const char*& aval, bool apop/* = true*/)
	{
		if (!isstring(L))
		{
			return false;
		}
		aval = lua_tostring(L, -1);
		if (aval == nullptr)
		{
			return false;
		}
		if (apop)
		{
			lua_pop(L, 1);
		}
		return true;
	}

	bool luaapi::isboolean(lua_State* L)
	{
		return lua_isboolean(L, -1);
	}

	void luaapi::pushboolean(lua_State* L, bool aval)
	{
		lua_pushboolean(L, aval);
	}

	bool luaapi::popboolean(lua_State* L, bool& aval, bool apop/* = true*/)
	{
		if (!isboolean(L))
		{
			return false;
		}
		aval = lua_toboolean(L, -1);
		if (apop)
		{
			lua_pop(L, 1);
		}
		return true;
	}

	void luaapi::register_func(lua_State* L, const char* afuncname, lua_CFunction fn)
	{
		lua_register(L, afuncname, fn);
	}

	void luaapi::add_package_path(lua_State* L, const char* apath)
	{
		lua_getglobal(L, "package");
		lua_getfield(L, -1, "path");
		std::string lpath = lua_tostring(L, -1);

		// 添加自定义路径（保留原有路径）
		lpath += std::format(";{}?.lua", apath);

		lua_pop(L, 1);
		lua_pushstring(L, lpath.c_str());
		lua_setfield(L, -2, "path");
		lua_pop(L, 1);
	}
}//namespace ngl