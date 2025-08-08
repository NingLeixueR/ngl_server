#pragma once

#include "luafunction.h"

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

			k2 ltemp1
			{
				.m_v1 = 1999,
				.m_v2 = k1{
					.m_v1 = 1988,
					.m_v2 = 5.134,
					.m_v3 = "chinachina",
					.m_v4 = {1,3,5,7,9,11},
					.m_v5 = {
						{1, "t1"},
						{2, "t2"},
						{3, "t3"},
					},
					.m_v6 = {
						{ "wac", "th1"},
						{ "lzs", "th2"},
						{ "lzm", "th3"},
						{ "lb", "th4"},
					},
					.m_v7 = {
						{"china", k0{.m_v1 = 1,.m_v2 = 2.3, .m_v3 = "china"}},
						{"jp", k0{.m_v1 = 2,.m_v2 = 3.3, .m_v3 = "jp"}},
					},
				},
			};
			//ltemp1.push(L);
			std::cout << std::endl;
			luafunction lfun(L, m_scriptpath.c_str(), "test");
			lfun.set_call(ltemp1);
			k2 ltemp2;
			lfun.set_return(ltemp2);
			lfun.call();
		}

		virtual bool init_sysdata(const char* asysjson)
		{
			luafunction lfun(L, m_scriptpath.c_str(), "init_sysdata");
			lfun.set_call(asysjson);
			lfun.set_return();
			return lfun.call();
		}

		// # 将db数据压入
		virtual bool data_push(const char* aname, const char* asource, const char* ajson, bool aedit)
		{
			luafunction lfun(L, m_scriptpath.c_str(), "data_push");
			lfun.set_call(aname, asource, ajson, aedit);
			lfun.set_return();
			return lfun.call();
		}

		// # 数据被删除
		virtual bool data_del(const char* aname, i64_actorid adataid)
		{
			luafunction lfun(L, m_scriptpath.c_str(), "data_del");
			lfun.set_call(aname, tools::lexical_cast<std::string>(adataid));
			lfun.set_return();
			return lfun.call();
		}

		virtual bool db_loadfinish()
		{
			luafunction lfun(L, m_scriptpath.c_str(), "db_loadfinish");
			lfun.set_call();
			lfun.set_return();
			return lfun.call();
		}

		// # 消息处理
		virtual bool handle(const char* aname, const char* ajson)
		{
			luafunction lfun(L, m_scriptpath.c_str(), "handle");
			lfun.set_call(aname, ajson);
			lfun.set_return();
			return lfun.call();
		}

		// # 查询数据是否被修改#如果被修改就加载数据
		virtual bool data_checkout(const char* aname, i64_actorid adataid, std::string& ajson)
		{
			luafunction lfun(L, m_scriptpath.c_str(), "data_checkout");
			lfun.set_call(aname, tools::lexical_cast<std::string>(adataid));
			bool success = false;
			lfun.set_return(success, ajson);
			return lfun.call();
		}
		
		// # 查询数据是否被删除
		// # aactorid == nguid::make() 检查全部数据
		virtual bool data_checkdel(const char* aname, i64_actorid adataid, std::string& ajson)
		{
			luafunction lfun(L, m_scriptpath.c_str(), "data_checkdel");
			lfun.set_call(aname, tools::lexical_cast<std::string>(adataid));
			bool success = false;
			lfun.set_return(success, ajson);
			return lfun.call();
		}
	};
}//namespace ngl