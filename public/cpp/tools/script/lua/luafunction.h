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
#pragma once

#include "tools/script/lua/serialize_lua.h"

namespace ngl
{
	class luafunction
	{
		lua_State* L				= nullptr;
		const char* m_scriptpath	= nullptr;
		const char* m_function		= nullptr;
		int32_t m_callcount			= 0;
		int32_t m_returncount		= 0;
		std::function<bool()> m_returnfunction = nullptr;
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
				lua_pop(L, 1); // 弹出错误信息
				return false;
			}
			if (lua_gettop(L) < m_returncount)
			{
				LOG_SCRIPT("{}.{} return count=[{},{}] error", m_scriptpath, m_function, lua_gettop(L), m_returncount);
				lua_settop(L, -lua_gettop(L));
				return false;
			}
			return m_returnfunction();
		}
	};
}//namespace ngl