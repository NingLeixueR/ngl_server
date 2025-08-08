#pragma once

#include "serialize_lua.h"

namespace ngl
{
	class luafunction
	{
		lua_State* L				= nullptr;
		const char* m_scriptpath	= nullptr;
		const char* m_function		= nullptr;
		int32_t m_callcount			= 0;
		int32_t m_returncount		= 0;
		std::function<bool()> m_returnfunction;
	public:
		luafunction(lua_State* aL, const char* ascriptpath, const char* afunction) :
			L(aL),
			m_scriptpath(ascriptpath),
			m_function(afunction)
		{}

		template <typename ...TARGS>
		bool set_call(const TARGS&... args)
		{
			if (lua_getglobal(L, m_function) == LUA_TNIL)
			{
				LOG_SCRIPT("lua_getglobal get failure[{}.{}]", m_scriptpath, m_function);
				lua_pop(L, 1);
			}
			m_callcount = sizeof...(TARGS);
			nlua_stack::stack_push(L, args...);
			return true;
		}

		template <typename ...TARGS>
		bool set_return(TARGS&... args)
		{
			m_returncount = sizeof...(TARGS);
			m_returnfunction = [this, &args...]()->bool
				{
					return nlua_stack::stack_pop(L, args...);
				};
			return true;
		}

		bool call()
		{
			if (lua_pcall(L, m_callcount, m_returncount, 0) != LUA_OK)
			{
				LOG_SCRIPT("{}.{} error [{}]", m_scriptpath, m_function, lua_tostring(L, -1));
				lua_pop(L, 1); // µ¯³ö´íÎóÐÅÏ¢
				return false;
			}
			if (lua_gettop(L) < m_returncount)
			{
				LOG_SCRIPT("{}.{} return count=[{},{}] error", lua_gettop(L), m_returncount);
				lua_settop(L, -lua_gettop(L));
				return false;
			}
			return m_returnfunction();
		}
	};
}//namespace ngl