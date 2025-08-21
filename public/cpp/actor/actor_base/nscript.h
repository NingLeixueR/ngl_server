#pragma once

#include "luafunction.h"
#include "actor_base.h"
#include "test_lua.h"
#include "ndefine.h"

extern "C"
{
	extern int nguidstr2int64(lua_State* L);

	// # lua发送协议给客户端
	// # parm 1 nguid(actor_type#areaid#dataid)
	// # parm 2 msgname
	// # parm 3 lua table
	extern int send_client(lua_State* L);

	// # lua发送协议给其他actor
	// # parm 1 nguid(actor_type#areaid#dataid)
	// # parm 2 msgname
	// # parm 3 lua table
	extern int send_actor(lua_State* L);
}

namespace ngl
{
	class actor_base;

	struct nscript_sysdata
	{
		int64_t m_nguid;

		def_nlua_function2("m_nguid", m_nguid)
	};

	template <enscript ESCRIPT>
	class nscript
	{
	public:
		static enscript type()
		{
			return ESCRIPT;
		}

		template <typename T>
		bool init_sysdata(const T& asys)
		{
			return false;
		}

		bool init(const char* asubdirectory, const char* ascript)
		{
			return false;
		}

		template <typename T>
		bool data_push(const char* aname, const char* asource, const T& ajson, bool aedit)
		{
			return false;
		}

		bool db_loadfinish()
		{
			return false;
		}

		template <typename T>
		bool handle(const char* aname, const T& ajson)
		{
			return false;
		}

		// # 数据被删除
		bool data_del(const char* aname, int64_t adataid)
		{
			return false;
		}

		// # 检查数据是否被修改
		template <typename T>
		bool data_checkout(const char* aname, i64_actorid adataid, T& adata)
		{
			return false;
		}

		template <typename T>
		bool data_checkout(const char* aname, std::map<int64_t, T>& adata)
		{
			return false;
		}

		// # 检查数据是否被删除
		bool data_checkdel(const char* aname, int64_t adataid)
		{
			return false;
		}

		bool data_checkdel(const char* aname, std::vector<int64_t>& adataid)
		{
			return false;
		}
	};

	template <>
	class nscript<enscript_lua>
	{
		lua_State* L = nullptr;
		std::string m_scriptpath;
	public:
		template <typename T>
		bool init_sysdata(const T& asys)
		{
			nlua_stack::stack_push(L, asys);
			return true;
		}

		void setupluapaths()
		{
			luaapi::add_package_path(L, sysconfig::lua().c_str());

			luaapi::register_func(L, "nguidstr2int64", nguidstr2int64);
			luaapi::register_func(L, "send_client", send_client);
			luaapi::register_func(L, "send_actor", send_actor);
			luaapi::register_func(L, "nsp_auto_save", send_client);
		}

		bool init(const char* asubdirectory, const char* ascript)
		{
			L = luaL_newstate();
			luaL_openlibs(L);  // 打开标准库
			setupluapaths();

			if (luaL_loadfile(L, (sysconfig::lua() + "rfunction.lua").c_str()) || lua_pcall(L, 0, 0, 0))
			{
				LOG_SCRIPT("can't run [{}#{}] : {} !", asubdirectory, m_scriptpath, lua_tostring(L, -1));
				lua_close(L);
				L = nullptr;
				return false;
			}
			m_scriptpath = std::format("../script/lua/{}/{}", asubdirectory, ascript);
			if (luaL_loadfile(L, m_scriptpath.c_str()) || lua_pcall(L, 0, 0, 0))
			{
				LOG_SCRIPT("can't run [{}#{}] : {} !", asubdirectory, m_scriptpath, lua_tostring(L, -1));
				lua_close(L);
				L = nullptr;
				return false;
			}
			return true;
		}

		template <typename T>
		bool data_push(const char* aname, const char* asource, const T& adata, bool aedit)
		{
			luafunction lfun(L, m_scriptpath.c_str(), "data_push");
			lfun.set_call(aname, asource, adata, aedit);
			lfun.set_return();
			return lfun.call();
		}

		bool db_loadfinish()
		{
			luafunction lfun(L, m_scriptpath.c_str(), "db_loadfinish");
			lfun.set_call();
			lfun.set_return();
			return lfun.call();
		}

		template <typename T>
		bool handle(const char* aname, const T& adata)
		{
			luafunction lfun(L, m_scriptpath.c_str(), "handle");
			lfun.set_call(aname, adata);
			lfun.set_return();
			return lfun.call();
		}

		bool data_del(const char* aname, int64_t adataid)
		{
			luafunction lfun(L, m_scriptpath.c_str(), "data_del");
			lfun.set_call(aname, adataid);
			lfun.set_return();
			return lfun.call();
		}

		// # 检查数据是否被修改
		template <typename T>
		bool data_checkout(const char* aname, i64_actorid adataid, T& adata)
		{
			luafunction lfun(L, m_scriptpath.c_str(), "data_checkoutbyid");
			lfun.set_call(aname, adataid);
			bool success = false;
			lfun.set_return(success, adata);
			return lfun.call();
		}

		template <typename T>
		struct dmap_checkout
		{
			std::map<int64_t, T>& m_data;

			dmap_checkout(std::map<int64_t, T>& adata) :
				m_data(adata)
			{}

			def_nlua_push2(tools::type_name<T>().c_str(), m_data);
			def_nlua_pop2(tools::type_name<T>().c_str(), m_data);
		};

		template <typename T>
		bool data_checkout(const char* aname, std::map<int64_t, T>& adata)
		{
			luafunction lfun(L, m_scriptpath.c_str(), "data_checkout");
			lfun.set_call(aname);
			bool success = false;
			dmap_checkout<T> ltemp(adata);
			lfun.set_return(success, ltemp);
			return lfun.call();
		}

		// # 检查数据是否被删除
		bool data_checkdel(const char* aname, i64_actorid adataid)
		{
			luafunction lfun(L, m_scriptpath.c_str(), "data_checkdelbyid");
			lfun.set_call(aname, adataid);
			bool success = false;
			lfun.set_return(success);
			if (lfun.call())
			{
				return success;
			}
			return false;
		}

		bool data_checkdel(const char* aname, std::vector<int64_t>& adataid)
		{
			luafunction lfun(L, m_scriptpath.c_str(), "data_checkdel");
			lfun.set_call(aname);
			bool success = false;
			lfun.set_return(success, adataid);
			if (lfun.call())
			{
				return success;
			}
			return false;
		}
	};


	class nscript_manage
	{
	public:
		static void* malloc(enscript atype, const char* asubdirectory, const char* ascript)
		{
			if(atype == enscript_lua)
			{
				nscript<enscript_lua>* lpnscript =  new nscript<enscript_lua>();
				if (lpnscript->init(asubdirectory, ascript))
				{
					return lpnscript;
				}
			}
			return nullptr;
		}

		static void release(void* anscript)
		{
			if (anscript != nullptr)
			{
				delete anscript;
			}
		}

		template <typename T>
		static bool init_sysdata(enscript atype, void* anscript, const T& asys)
		{
			if (atype == enscript_lua)
			{
				auto lpnscript = (nscript<enscript_lua>*)(anscript);
				return lpnscript->init_sysdata(asys);
			}
			return false;
		}

		template <typename T>
		static bool data_push(enscript atype, void* anscript, const char* aname, const char* asource, const T& ajson, bool aedit)
		{
			if (atype == enscript_lua)
			{
				auto lpnscript = (nscript<enscript_lua>*)(anscript);
				return lpnscript->data_push(aname, asource, ajson, aedit);
			}
			return false;
		}

		static bool db_loadfinish(enscript atype, void* anscript)
		{
			if (atype == enscript_lua)
			{
				auto lpnscript = (nscript<enscript_lua>*)(anscript);
				return lpnscript->db_loadfinish();
			}
			return false;
		}

		template <typename T>
		static bool handle(enscript atype, void* anscript, const char* aname, const T& adata)
		{
			if (atype == enscript_lua)
			{
				auto lpnscript = (nscript<enscript_lua>*)(anscript);
				return lpnscript->handle(aname, adata);
			}
			return false;
		}

		static bool data_del(enscript atype, void* anscript, const char* aname, int64_t adataid)
		{
			if (atype == enscript_lua)
			{
				auto lpnscript = (nscript<enscript_lua>*)(anscript);
				return lpnscript->data_del(aname, adataid);
			}
			return false;
		}

		template <typename T>
		static bool data_checkout(enscript atype, void* anscript, const char* aname, i64_actorid adataid, T& adata)
		{
			if (atype == enscript_lua)
			{
				auto lpnscript = (nscript<enscript_lua>*)(anscript);
				return lpnscript->data_checkout(aname, adataid, adata);
			}
			return false;
		}

		template <typename T>
		static bool data_checkout(enscript atype, void* anscript, const char* aname, std::map<int64_t,T>& adata)
		{
			if (atype == enscript_lua)
			{
				auto lpnscript = (nscript<enscript_lua>*)(anscript);
				return lpnscript->data_checkout(aname, adata);
			}
			return false;
		}

		static bool data_checkdel(enscript atype, void* anscript, const char* aname, int64_t adataid)
		{
			if (atype == enscript_lua)
			{
				auto lpnscript = (nscript<enscript_lua>*)(anscript);
				return lpnscript->data_checkdel(aname, adataid);
			}
			return false;
		}

		static bool data_checkdel(
			enscript atype, void* anscript, const char* aname, std::vector<int64_t>& adelids
		)
		{
			if (atype == enscript_lua)
			{
				auto lpnscript = (nscript<enscript_lua>*)(anscript);
				return lpnscript->data_checkdel(aname, adelids);
			}
			return false;
		}
	};


}//namespace ngl