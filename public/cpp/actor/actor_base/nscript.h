#pragma once

#include "lua.hpp"
#include "nguid.h"

#include <string>
#include <map>

//# define LOG_SCRIPT(...)  ::ngl::log_error()->print(__VA_ARGS__)
# define LOG_SCRIPT(...)  std::cout<<std::format(__VA_ARGS__)

extern "C"
{
	extern int nguidstr2int64str(lua_State* L);

	// # lua发送协议给客户端
	// # parm 1 nguid(actor_type#areaid#dataid)
	// # parm 2 msgname
	// # parm 3 jsonmsg 
	extern int send_client(lua_State* L);

	// # lua发送协议给其他actor
	// # parm 1 nguid(actor_type#areaid#dataid)
	// # parm 2 msgname
	// # parm 3 jsonmsg 
	extern int send_actor(lua_State* L);

}

namespace ngl
{
	class actor_base;

	enum enscript
	{
		enscript_none,
		enscript_lua,
	};

	struct nscript_sysdata
	{
		std::string m_nguid;

		dprotocoljson(nscript_sysdata, m_nguid)
	};

	class nscript
	{
		virtual std::shared_ptr<nscript> mscript(actor_base*) = 0;
		static std::map<enscript, nscript*> m_data;
		enscript m_type;
		actor_base* m_actor;
	public:
		nscript(enscript atype, actor_base* aactor, bool amallocinit = false);

		actor_base* get_actor()
		{
			return m_actor;
		}

		enscript type()
		{
			return m_type;
		}

		virtual bool init_sysdata(const std::string& asysjson) = 0;
		virtual void init(const std::string& ascript) = 0;
		virtual bool push_data(const std::string& adbname, const std::string& adatajson, bool aedit) = 0;
		virtual bool handle(const std::string& aname, const std::string& ajson) = 0;
		virtual bool check_outdata(const std::string& adbname, i64_actorid aactorid, std::string& adatajson) = 0;

		static std::shared_ptr<nscript> malloc_script(enscript atype, actor_base* aactor)
		{
			auto itor = m_data.find(atype);
			if (itor == m_data.end())
			{
				return nullptr;
			}
			return itor->second->mscript(aactor);
		}
	};

	class nscriptlua
		:public nscript
	{
		lua_State* L = nullptr;
		std::string m_scriptpath;//脚本文件名称

		virtual std::shared_ptr<nscript> mscript(actor_base* aactor)
		{
			return std::make_shared<nscriptlua>(aactor);
		}
	public:
		nscriptlua(actor_base* aactor, bool amallocinit = false) :
			nscript(enscript_lua, aactor, amallocinit)
		{}

		virtual ~nscriptlua()
		{
			lua_close(L);
		}

		void setupluapaths()
		{
			lua_getglobal(L, "package");
			lua_getfield(L, -1, "path");
			std::string lpath = lua_tostring(L, -1);

			// 添加自定义路径（保留原有路径）
			lpath += std::format(";{}?.lua", sysconfig::lua());

			lua_pop(L, 1);
			lua_pushstring(L, lpath.c_str());
			lua_setfield(L, -2, "path");
			lua_pop(L, 1);

			lua_register(L, "nguidstr2int64str", nguidstr2int64str);
			lua_register(L, "send_client", send_client);
			lua_register(L, "send_actor", send_client);		
		}

		virtual void init(const std::string& ascript)
		{
			L = luaL_newstate();
			luaL_openlibs(L);  // 打开标准库
			setupluapaths();
			
			if (luaL_loadfile(L, (sysconfig::lua() + "rfunction.lua").c_str()) || lua_pcall(L, 0, 0, 0))
			{
				LOG_SCRIPT("can't run [{}] : {} !", m_scriptpath, lua_tostring(L, -1));
				lua_close(L);
				L = nullptr;
				return;
			}
			m_scriptpath = std::format("../script/lua/{}", ascript);
			if (luaL_loadfile(L, m_scriptpath.c_str()) || lua_pcall(L, 0, 0, 0))
			{
				LOG_SCRIPT("can't run [{}] : {} !", m_scriptpath, lua_tostring(L, -1));
				lua_close(L);
				L = nullptr;
				return;
			}
		}

		virtual bool init_sysdata(const std::string& asysjson)
		{
			if (L == nullptr)
			{
				return false;
			}
			int result = lua_getglobal(L, "init_sysdata");
			if (result == LUA_TNIL)
			{
				LOG_SCRIPT("lua_getglobal get failure[{}.init_sysdata]", m_scriptpath);
				lua_pop(L, 1); // 弹出nil值
				return false;
			}
			lua_pushstring(L, asysjson.c_str());
			if (lua_pcall(L, 1, 0, 0) != LUA_OK)
			{
				LOG_SCRIPT("{}.init_sysdata error [{}]", m_scriptpath, lua_tostring(L, -1));
				lua_pop(L, 1); // 弹出错误信息
				return false;
			}
			return true;
		}

		// # 将db数据压入
		virtual bool push_data(const std::string& adbname, const std::string& adatajson, bool aedit)
		{
			if (L == nullptr)
			{
				return false;
			}
			int result = lua_getglobal(L, "push_data");
			if (result == LUA_TNIL)
			{
				LOG_SCRIPT("lua_getglobal get failure[{}.push_data]", m_scriptpath);
				lua_pop(L, 1); // 弹出nil值
				return false;
			}
			lua_pushstring(L, adbname.c_str());
			lua_pushstring(L, adatajson.c_str());
			lua_pushboolean(L, aedit);
			if (lua_pcall(L, 3, 0, 0) != LUA_OK)
			{
				LOG_SCRIPT("{}.push_data error [{}]", m_scriptpath, lua_tostring(L, -1));
				lua_pop(L, 1); // 弹出错误信息
				return false;
			}
			return true;
		}

		// # 消息处理
		virtual bool handle(const std::string& aname, const std::string& ajson)
		{
			if (L == nullptr)
			{
				return false;
			}
			int result = lua_getglobal(L, "handle");
			if (result == LUA_TNIL)
			{
				LOG_SCRIPT("lua_getglobal get failure[{}.handle]", m_scriptpath);
				lua_pop(L, 1); // 弹出nil值
				return false;
			}
			lua_pushstring(L, aname.c_str());
			lua_pushstring(L, ajson.c_str());
			if (lua_pcall(L, 2, 0, 0) != LUA_OK)
			{
				LOG_SCRIPT("{}.handle error [{}]", m_scriptpath, lua_tostring(L, -1));
				lua_pop(L, 1); // 弹出错误信息
				return false;
			}

			return true;
		}

		// # 查询数据是否被修改#如果被修改就加载数据
		// # aactorid == nguid::make() 检查全部数据
		virtual bool check_outdata(const std::string& adbname, i64_actorid aactorid, std::string& adatajson)
		{
			if (L == nullptr)
			{
				return false;
			}

			int result = lua_getglobal(L, "check_outdata");
			if (result == LUA_TNIL)
			{
				LOG_SCRIPT("lua_getglobal get failure[{}.{}]", m_scriptpath, "check_outdata");
				lua_pop(L, 1);
				return false;
			}
			lua_pushstring(L, adbname.c_str());
			lua_pushstring(L, tools::lexical_cast<std::string>(aactorid).c_str());
			if (lua_pcall(L, 2, 2, 0) != LUA_OK)
			{
				LOG_SCRIPT("{}.check_outdata error [{}]", m_scriptpath, lua_tostring(L, -1));
				lua_pop(L, 1);
				return false;
			}

			if (lua_gettop(L) < 2)
			{
				LOG_SCRIPT("{}.check_outdata return count error", m_scriptpath);
				lua_settop(L, -lua_gettop(L));
				return false;
			}

			if (!lua_isboolean(L, -2))
			{
				LOG_SCRIPT("{}.check_outdata return error isboolean", m_scriptpath);
				lua_settop(L, -lua_gettop(L));
				return false;
			}

			if (!lua_isstring(L, -1))
			{
				LOG_SCRIPT("{}.check_outdata return error isstring", m_scriptpath);
				lua_pop(L, 1);
				return false;
			}

			bool success = lua_toboolean(L, -2) != 0;
			if (success)
			{
				adatajson = lua_tostring(L, -1);
			}
			lua_pop(L, 2);

			return success;
		}
	};
}//namespace ngl