#pragma once

#include "lua.hpp"
#include "nguid.h"

#include <string>
#include <map>

//# define LOG_SCRIPT(...)  ::ngl::log_error()->print(__VA_ARGS__)
# define LOG_SCRIPT(...)  std::cout << std::format(__VA_ARGS__) << std::endl

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

	class luatools
	{
	public:
		// 保存 void* 到 Lua
		template <typename T>
		static void set_pointer(lua_State* L, T* ptr, int64_t aid = 0)
		{
			std::string lname = std::format("{}_{}", tools::type_name<T>(), aid);
			lua_pushlightuserdata(L, ptr);
			lua_setglobal(L, lname.c_str());
		}

		// 从 Lua 获取 void*
		template <typename T>
		static T* get_pointer(lua_State* L, int64_t aid = 0)
		{
			std::string lname = std::format("{}_{}", tools::type_name<T>(), aid);
			lua_getglobal(L, lname.c_str());  // 获取全局变量
			if (lua_islightuserdata(L, -1))
			{
				return (T*)lua_touserdata(L, -1);  // 返回指针
			}
			return nullptr;  // 类型不匹配
		}
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

		// # 初始化一些杂项
		virtual bool init_sysdata(const char* asysjson) = 0;

		// # lua初始化
		virtual void init(const char* asubdirectory, const char* ascript) = 0;

		// # 将数据压入lua
		virtual bool data_push(const char* aname, const char* asource, const char* ajson, bool aedit) = 0;

		// # 数据加载完成
		virtual bool db_loadfinish() = 0;

		// # lua消息处理
		virtual bool handle(const char* aname, const char* ajson) = 0;

		// # 数据被删除
		virtual bool data_del(const char* aname, i64_actorid aactorid) = 0;

		// # 检查数据是否被修改
		virtual bool data_checkout(const char* aname, i64_actorid adataid, std::string& ajson) = 0;

		// # 检查数据是否被删除
		virtual bool data_checkdel(const char* aname, i64_actorid adataid, std::string& ajson) = 0;

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
			lua_register(L, "send_actor", send_actor);
			lua_register(L, "nsp_auto_save", send_client);			
		}

		virtual void init(const char* asubdirectory, const char* ascript)
		{
			L = luaL_newstate();
			luaL_openlibs(L);  // 打开标准库
			setupluapaths();

			luatools::set_pointer(L, get_actor());

			if (luaL_loadfile(L, (sysconfig::lua() + "rfunction.lua").c_str()) || lua_pcall(L, 0, 0, 0))
			{
				LOG_SCRIPT("can't run [{}#{}] : {} !", asubdirectory, m_scriptpath, lua_tostring(L, -1));
				lua_close(L);
				L = nullptr;
				return;
			}
			m_scriptpath = std::format("../script/lua/{}/{}", asubdirectory, ascript);
			if (luaL_loadfile(L, m_scriptpath.c_str()) || lua_pcall(L, 0, 0, 0))
			{
				LOG_SCRIPT("can't run [{}#{}] : {} !", asubdirectory, m_scriptpath, lua_tostring(L, -1));
				lua_close(L);
				L = nullptr;
				return;
			}
		}

		virtual bool init_sysdata(const char* asysjson)
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
			lua_pushstring(L, asysjson);
			if (lua_pcall(L, 1, 0, 0) != LUA_OK)
			{
				LOG_SCRIPT("{}.init_sysdata error [{}]", m_scriptpath, lua_tostring(L, -1));
				lua_pop(L, 1); // 弹出错误信息
				return false;
			}
			return true;
		}

		// # 将db数据压入
		virtual bool data_push(const char* aname, const char* asource, const char* ajson, bool aedit)
		{
			if (L == nullptr)
			{
				return false;
			}
			int result = lua_getglobal(L, "data_push");
			if (result == LUA_TNIL)
			{
				LOG_SCRIPT("lua_getglobal get failure[{}.push_data]", m_scriptpath);
				lua_pop(L, 1); // 弹出nil值
				return false;
			}
			lua_pushstring(L, aname);
			lua_pushstring(L, asource);
			lua_pushstring(L, ajson);
			lua_pushboolean(L, aedit);
			if (lua_pcall(L, 4, 0, 0) != LUA_OK)
			{
				LOG_SCRIPT("{}.push_data error [{}]", m_scriptpath, lua_tostring(L, -1));
				lua_pop(L, 1); // 弹出错误信息
				return false;
			}
			return true;
		}

		// # 数据被删除
		virtual bool data_del(const char* aname, i64_actorid adataid)
		{
			if (L == nullptr)
			{
				return false;
			}
			int result = lua_getglobal(L, "data_del");
			if (result == LUA_TNIL)
			{
				LOG_SCRIPT("lua_getglobal get failure[{}.del_data]", m_scriptpath);
				lua_pop(L, 1); // 弹出nil值
				return false;
			}
			lua_pushstring(L, aname);
			lua_pushstring(L, tools::lexical_cast<std::string>(adataid).c_str());
			if (lua_pcall(L, 2, 0, 0) != LUA_OK)
			{
				LOG_SCRIPT("{}.del_data error [{}]", m_scriptpath, lua_tostring(L, -1));
				lua_pop(L, 1); // 弹出错误信息
				return false;
			}
			return true;
		}

		virtual bool db_loadfinish()
		{
			if (L == nullptr)
			{
				return false;
			}
			int result = lua_getglobal(L, "db_loadfinish");
			if (result == LUA_TNIL)
			{
				LOG_SCRIPT("lua_getglobal get failure[{}.db_loadfinish]", m_scriptpath);
				lua_pop(L, 1); // 弹出nil值
				return false;
			}
			if (lua_pcall(L, 0, 0, 0) != LUA_OK)
			{
				LOG_SCRIPT("{}.db_loadfinish error [{}]", m_scriptpath, lua_tostring(L, -1));
				lua_pop(L, 1); // 弹出错误信息
				return false;
			}

			return true;
		}

		// # 消息处理
		virtual bool handle(const char* aname, const char* ajson)
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
			lua_pushstring(L, aname);
			lua_pushstring(L, ajson);
			if (lua_pcall(L, 2, 0, 0) != LUA_OK)
			{
				LOG_SCRIPT("{}.handle error [{}]", m_scriptpath, lua_tostring(L, -1));
				lua_pop(L, 1); // 弹出错误信息
				return false;
			}

			return true;
		}

		// # 查询数据是否被修改#如果被修改就加载数据
		virtual bool data_checkout(const char* adbname, i64_actorid adataid, std::string& ajson)
		{
			if (L == nullptr)
			{
				return false;
			}

			int result = lua_getglobal(L, "data_checkout");
			if (result == LUA_TNIL)
			{
				LOG_SCRIPT("lua_getglobal get failure[{}.check_outdata]", m_scriptpath);
				lua_pop(L, 1);
				return false;
			}
			lua_pushstring(L, adbname);
			lua_pushstring(L, tools::lexical_cast<std::string>(adataid).c_str());
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
				ajson = lua_tostring(L, -1);
			}
			lua_pop(L, 2);

			return success;
		}
		
		// # 查询数据是否被删除
		// # aactorid == nguid::make() 检查全部数据
		virtual bool data_checkdel(const char* adbname, i64_actorid adataid, std::string& ajson)
		{
			if (L == nullptr)
			{
				return false;
			}

			int result = lua_getglobal(L, "data_checkdel");
			if (result == LUA_TNIL)
			{
				LOG_SCRIPT("lua_getglobal get failure[{}.data_checkdel]", m_scriptpath);
				lua_pop(L, 1);
				return false;
			}
			lua_pushstring(L, adbname);
			lua_pushstring(L, tools::lexical_cast<std::string>(adataid).c_str());
			if (lua_pcall(L, 2, 2, 0) != LUA_OK)
			{
				LOG_SCRIPT("{}.data_checkdel error [{}]", m_scriptpath, lua_tostring(L, -1));
				lua_pop(L, 1);
				return false;
			}

			if (lua_gettop(L) < 2)
			{
				LOG_SCRIPT("{}.data_checkdel return count error", m_scriptpath);
				lua_settop(L, -lua_gettop(L));
				return false;
			}

			if (!lua_isboolean(L, -2))
			{
				LOG_SCRIPT("{}.data_checkdel return error isboolean", m_scriptpath);
				lua_settop(L, -lua_gettop(L));
				return false;
			}

			if (!lua_isstring(L, -1))
			{
				LOG_SCRIPT("{}.data_checkdel return error isstring", m_scriptpath);
				lua_pop(L, 1);
				return false;
			}

			bool success = lua_toboolean(L, -2) != 0;
			if (success)
			{
				ajson = lua_tostring(L, -1);
			}
			lua_pop(L, 2);

			return success;
		}
	};
}//namespace ngl