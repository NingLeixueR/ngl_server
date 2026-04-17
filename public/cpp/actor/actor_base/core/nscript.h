/*
* Copyright (c) [2020-2025] NingLeixueR
* 
* Project name: ngl_server
* Project URL: https://github.com/NingLeixueR/ngl_server
* 
* This file is part of the ngl_server project and is distributed under the MIT License.
* You may use, modify, and distribute this project under the license, including commercial use,
* but you must retain the original copyright and license notice.
* 
* For license details, see the LICENSE file in the project root:
* https://github.com/NingLeixueR/ngl_server/blob/main/LICENSE
*/
// File overview: Lua script binding component for actors with scripted behavior.

#pragma once

#include "actor/actor_base/core/actor_base.h"
#include "tools/script/lua/luafunction.h"
#include "tools/serialize/ndefine.h"

#include <memory>

extern "C"
{
	// Lua helper: convert `actor_type#area#dataid` text into packed nguid.
	extern int nguidstr2int64(lua_State* L);

	// Lua helper: send one protocol message to clients through the gateway path.
	extern int send_client(lua_State* L);

	// Lua helper: send one protocol message to another actor.
	extern int send_actor(lua_State* L);
}

namespace ngl
{
	class actor_base;

	class nscript_obj
	{
	public:
		virtual ~nscript_obj() = default;
		virtual enscript type() const noexcept = 0;
	};

	struct nscript_sysdata
	{
		i64_actorid m_nguid = nguid::make();

		DEF_NLUA_SPECIAL_FUNCTION({ "m_nguid" }, m_nguid)
	};

	template <enscript ESCRIPT>
	class nscript :
		public nscript_obj
	{
	public:
		enscript type() const noexcept override
		{
			return ESCRIPT;
		}

		// Push process/actor system data into the script runtime.
		template <typename T>
		bool init_sysdata(const T& asys)
		{
			return false;
		}

		// Create the runtime and load the backing script file.
		bool init(const char* asubdirectory, const char* ascript)
		{
			return false;
		}

		// Notify the script layer that one DB row was created/updated.
		template <typename T>
		bool data_push(const char* aname, const char* asource, const T& adata, bool aedit)
		{
			return false;
		}

		// Notify the script layer that the initial DB load is complete.
		bool db_loadfinish()
		{
			return false;
		}

		// Dispatch one actor message into the script layer.
		template <typename T>
		bool handle(const char* aname, const T& adata)
		{
			return false;
		}

		// Notify the script layer that one row was deleted.
		bool data_del(const char* aname, int64_t adataid)
		{
			return false;
		}

		// Pull a possibly edited row back out of the script layer.
		template <typename T>
		bool data_checkout(const char* aname, i64_actorid adataid, T& adata)
		{
			return false;
		}

		// Pull all edited rows of one table back from the script layer.
		template <typename T>
		bool data_checkout(const char* aname, std::map<int64_t, T>& adata)
		{
			return false;
		}

		// Check whether one row has been marked deleted in script state.
		bool data_checkdel(const char* aname, int64_t adataid)
		{
			return false;
		}

		// Collect rows that were marked deleted in script state.
		bool data_checkdel(const char* aname, std::vector<int64_t>& adataid)
		{
			return false;
		}
	};

	template <>
	class nscript<enscript_lua>
		:
		public nscript_obj
	{
		nscript(const nscript&) = delete;
		nscript& operator=(const nscript&) = delete;
		nscript(nscript&&) = delete;
		nscript& operator=(nscript&&) = delete;

		lua_State* L = nullptr;
		std::string m_scriptpath;

		void close_state() noexcept
		{
			if (L != nullptr)
			{
				lua_close(L);
				L = nullptr;
			}
		}
	public:
		nscript() = default;

		enscript type() const noexcept override
		{
			return enscript_lua;
		}

		~nscript()
		{
			close_state();
		}

		template <typename T>
		bool init_sysdata(const T& asys)
		{
			nlua_stack::stack_push(L, asys);
			return true;
		}

		void setupluapaths()
		{
			// Register project-specific search paths and native helper functions
			// before loading user scripts.
			luaapi::add_package_path(L, sysconfig::lua().c_str());
			luaapi::register_func(L, "nguidstr2int64", nguidstr2int64);
			luaapi::register_func(L, "send_client", send_client);
			luaapi::register_func(L, "send_actor", send_actor);
		}

		bool init(const char* asubdirectory, const char* ascript)
		{
			close_state();
			L = luaL_newstate();
			if (L == nullptr)
			{
				return false;
			}
			luaL_openlibs(L);  // Expose the standard Lua libraries.
			setupluapaths();

			if (luaL_loadfile(L, (sysconfig::lua() + "rfunction.lua").c_str()) || lua_pcall(L, 0, 0, 0))
			{
				LOG_SCRIPT("can't run [{}#{}] : {} !", asubdirectory, m_scriptpath, lua_tostring(L, -1));
				close_state();
				return false;
			}
			m_scriptpath = std::format("../configure/script/lua/{}/{}", asubdirectory, ascript);
			if (luaL_loadfile(L, m_scriptpath.c_str()) || lua_pcall(L, 0, 0, 0))
			{
				LOG_SCRIPT("can't run [{}#{}] : {} !", asubdirectory, m_scriptpath, lua_tostring(L, -1));
				close_state();
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

		// Call Lua `data_checkoutbyid` and read back a success flag plus row data.
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

			explicit dmap_checkout(std::map<int64_t, T>& adata) :
				m_data(adata)
			{}

			DEF_NLUA_SPECIAL_FUNCTION({ tools::type_name<T>().c_str() }, m_data)
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

		// Call Lua `data_checkdelbyid` and return the boolean result.
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
		static nscript<enscript_lua>* lua_ptr(enscript atype, nscript_obj* ascript)
		{
			if (atype != enscript_lua || ascript == nullptr)
			{
				return nullptr;
			}
			if (ascript->type() == enscript_lua)
			{
				return static_cast<nscript<enscript_lua>*>(ascript);
			}
			return nullptr;
		}
	public:
		using script_ptr = std::unique_ptr<nscript_obj>;

		// Backend-neutral factory and dispatch helpers used by actor modules.
		static script_ptr make(enscript atype, const char* asubdirectory, const char* ascript)
		{
			if(atype == enscript_lua)
			{
				auto lpnscript = std::make_unique<nscript<enscript_lua>>();
				if (!lpnscript->init(asubdirectory, ascript))
				{
					return nullptr;
				}
				return lpnscript;
			}
			return nullptr;
		}

		template <typename T>
		static bool init_sysdata(enscript atype, nscript_obj* anscript, const T& asys)
		{
			auto* lptr = lua_ptr(atype, anscript);
			if (lptr == nullptr)
			{
				return false;
			}
			return lptr->init_sysdata(asys);
		}

		template <typename T>
		static bool data_push(enscript atype, nscript_obj* anscript, const char* aname, const char* asource, const T& ajson, bool aedit)
		{
			auto* lptr = lua_ptr(atype, anscript);
			if (lptr == nullptr)
			{
				return false;
			}
			return lptr->data_push(aname, asource, ajson, aedit);
		}

		static bool db_loadfinish(enscript atype, nscript_obj* anscript)
		{
			auto* lptr = lua_ptr(atype, anscript);
			if (lptr == nullptr)
			{
				return false;
			}
			return lptr->db_loadfinish();
		}

		template <typename T>
		static bool handle(enscript atype, nscript_obj* anscript, const char* aname, const T& adata)
		{
			auto* lptr = lua_ptr(atype, anscript);
			if (lptr == nullptr)
			{
				return false;
			}
			return lptr->handle(aname, adata);
		}

		static bool data_del(enscript atype, nscript_obj* anscript, const char* aname, int64_t adataid)
		{
			auto* lptr = lua_ptr(atype, anscript);
			if (lptr == nullptr)
			{
				return false;
			}
			return lptr->data_del(aname, adataid);
		}

		template <typename T>
		static bool data_checkout(enscript atype, nscript_obj* anscript, const char* aname, i64_actorid adataid, T& adata)
		{
			auto* lptr = lua_ptr(atype, anscript);
			if (lptr == nullptr)
			{
				return false;
			}
			return lptr->data_checkout(aname, adataid, adata);
		}

		template <typename T>
		static bool data_checkout(enscript atype, nscript_obj* anscript, const char* aname, std::map<int64_t,T>& adata)
		{
			auto* lptr = lua_ptr(atype, anscript);
			if (lptr == nullptr)
			{
				return false;
			}
			return lptr->data_checkout(aname, adata);
		}

		static bool data_checkdel(enscript atype, nscript_obj* anscript, const char* aname, int64_t adataid)
		{
			auto* lptr = lua_ptr(atype, anscript);
			if (lptr == nullptr)
			{
				return false;
			}
			return lptr->data_checkdel(aname, adataid);
		}

		static bool data_checkdel(enscript atype, nscript_obj* anscript, const char* aname, std::vector<int64_t>& adelids)
		{
			auto* lptr = lua_ptr(atype, anscript);
			if (lptr == nullptr)
			{
				return false;
			}
			return lptr->data_checkdel(aname, adelids);
		}
	};
}//namespace ngl
