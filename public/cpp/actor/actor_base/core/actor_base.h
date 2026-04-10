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
// File overview: Declares interfaces for actor base.

#pragma once

#include "actor/actor_base/core/nmanage_dbclient.h"
#include "actor/actor_base/core/handle_pram.h"
#include "actor/actor_base/core/nactortype.h"
#include "actor/actor_base/core/nscript.h"
#include "actor/actor_base/core/ntimer.h"
#include "actor/actor_base/core/nguid.h"
#include "tools/tools/tools_group.h"
#include "tools/tools/tools_time.h"
#include "actor/tab/ttab_servers.h"
#include "net/udp/kcp/ukcp.h"
#include "tools/type.h"

#include <functional>
#include <optional>
#include <memory>
#include <vector>
#include <atomic>
#include <list>
#include <set>
#include <map>

namespace ngl
{
	class actor_base;
	class ndb_component;
	class ndbclient_base;
	class nmanage_dbclient;

	template <typename TDBTAB>
	struct data_modified;

	struct actorparmbase
	{
		ENUM_ACTOR		m_type				= nguid::none_type();				// Actor type.
		i16_area		m_area				= tab_self_area;					// Logical area/shard.
		i32_actordataid m_id				= nguid::none_actordataid();		// Actor-local numeric id.
		bool			m_manage_dbclient	= false;							// Create the dbclient manager for this actor.
		enscript		m_enscript			= enscript_none;					// Optional scripting backend.
	};

	// Scheduler-visible actor lifecycle state.
	enum actor_stat
	{
		actor_stat_init,  // Initialization stage
		actor_stat_free,  // Idle state
		actor_stat_list,  // Queued in actor_manage and waiting for a worker.
		actor_stat_run,	  // Currently executing on a worker thread.
		actor_stat_close, // Closed state
	};

	// Strongly typed message envelope declared in handle_pram-related code.
	template <typename T>
	struct message;

	// High privilege threshold
	enum e_hightlevel
	{
		e_hightlevel_db = 1,
		e_hightlevel_nsp = 2,
		e_hightlevel_custom = 10,
	};

	/**
	 * Actor readiness state checker.
	 *
	 * Purpose: combine the three states "database load complete", "NSP sync complete", and "custom condition satisfied".
	 * Convert them into bit flags and use is_ready() to determine whether every required state is ready.
	 * Usage: before dispatching a message to an Actor, use this checker to verify that the Actor is ready to process it.
	 */
	class nready
	{
		struct ready
		{
			std::string m_name;
			std::function<bool()> m_fun;
		};
		std::map<int32_t, std::vector<ready>> m_ready;
	public:
		// Return true when the requested readiness bits are satisfied.
		bool is_ready();

		int32_t hightlevel_ready();

		// Register a readiness predicate for a predefined readiness bit.
		template <typename ...ARGS>
		void set_ready(const std::string& akey, const std::function<bool()>& afun, e_hightlevel ahightlevel)
		{
			auto& lvec = m_ready[ahightlevel+1];
			lvec.emplace_back(ready
				{
					.m_name = akey,
					.m_fun = afun,
				}
			);
			if constexpr (sizeof...(ARGS) > 0)
			{
				if (!ngl::tprotocol::set_hightvalue<ARGS...>(ahightlevel))
				{
					log_error()->print("nready set_ready fail!");
				}
			}
		}
	};

	template <typename T>
	struct nscript_data_csv
	{
		using TDATA = T;
		const std::map<int32_t, T>& data;

		explicit nscript_data_csv(const std::map<int32_t, T>& adata) :
			data(adata)
		{}

		DPROTOCOL(nscript_data_csv<T>, data)
	};

	template <typename T>
	struct nscript_data_db
	{
		using TDATA = T;
		std::map<int64_t, T*> data;

		DPROTOCOL(nscript_data_db<T>, data)
	};

	template <typename T>
	struct nscript_data_nsp
	{
		using TDATA = T;
		const std::map<int64_t, T>& data;
		explicit nscript_data_nsp(const std::map<int64_t, T>& adata) :
			data(adata)
		{}

		DPROTOCOL(nscript_data_nsp<T>, data)
	};

	struct kcpport
	{
		// Remote KCP type to local endpoint index mapping.
		std::map<pbnet::ENUM_KCP, i16_port> m_data;
	};

	class actor_base
	{
		nguid										m_guid = nguid::make();			// Immutable actor identity.
		std::unique_ptr<nmanage_dbclient>		m_dbclient = nullptr;			// Optional DB component manager.
		bool										m_isload = false;				// Whether this actor participates in DB loading.
		std::map<pbdb::ENUM_DB, ndb_component*>		m_dbcomponent;					// Registered DB-backed subcomponents.

		// Shared periodic broadcast tick used by actors that opt in for maintenance work.
		static int									m_broadcast;					// Broadcast interval in milliseconds.
		static int64_t								m_broadcasttimer;				// Timer id for the shared broadcast task.
		bool										m_isbroadcast = false;			// Whether this actor receives broadcast ticks.
				
		nready										m_ready;						// Readiness gate collection.
		nscript_manage::script_ptr				m_script = nullptr;				// Backend-neutral script runtime handle, if enabled.
		enscript									m_enscript = enscript_none;		// Active scripting backend.

		std::map<i32_serverid, kcpport>				m_kcpindex;					// Cached KCP routing metadata by server.
	public:
		explicit actor_base(const actorparmbase& aparm);

		// Access the actor's readiness gate collection.
		nready& ready();

		// Access the optional DB manager owned by this actor.
		std::unique_ptr<nmanage_dbclient>& manage_dbclient();

		// Register one DB-backed subcomponent with this actor.
		void set_db_component(ndb_component* acomponent);

		// Let all registered DB components populate their in-memory derived state.
		void db_component_init_data();

		// Initialize DB components either from persisted data or by creating fresh records.
		void init_db_component(bool acreate);

		// Attach a concrete dbclient instance to the actor-level DB manager.
		void add_dbclient(ndbclient_base* adbclient, i64_actorid aid);

		// Handle a DB load response and feed the result into the actor's DB components.
		template <pbdb::ENUM_DB DBTYPE, typename TDBTAB, typename TACTOR>
		bool handle_db(const message<np_actordb_load_response<DBTYPE, TDBTAB>>& adata);

		virtual ~actor_base();

		// Actor-specific startup hook. Derived classes normally bind dbclients here.
		virtual void init() {/* Function,if base class */ }

		// Return the scheduler-visible actor state.
		virtual actor_stat activity_stat() = 0;

		// Update the scheduler-visible actor state.
		virtual void set_activity_stat(actor_stat astat) = 0;

		// Return whether the actor currently has pending tasks.
		virtual bool list_empty() = 0;

		// Return whether the actor currently has pending high-priority tasks.
		virtual bool high_empty() = 0;

		// Process the actor's pending work on the given worker thread.
		virtual bool actor_handle(i32_threadid athreadid) = 0;

		// Enqueue one new message/task for this actor.
		virtual bool push(handle_pram& apram) = 0;

		// Optional hook after one handle_pram item is processed.
		virtual void handle_after(handle_pram&) {/* Function,if base class */ }

		// Optional hook triggered when a DB table finishes loading.
		virtual void loaddb_finish([[maybe_unused]] pbdb::ENUM_DB atype, [[maybe_unused]] enum_dbstat astat) {/* Function,if base class */ }

		// Destroy or recycle the actor implementation.
		virtual void release() = 0;

		// Optional cleanup hook before the actor is removed from actor_manage.
		virtual void erase_actor_before() {/* Function,if base class */ }

		// Flush DB-backed state through the actor's dbclient manager.
		virtual void save();

		// Return whether the actor type is a singleton actor.
		bool is_single()const;

		// Return the full actor guid object.
		const nguid& guid()const;

		// Return the actor guid encoded as i64_actorid.
		i64_actorid id_guid()const;

		// Return the actor-local numeric id.
		i32_actordataid id()const;

		// Return the logical area/shard id.
		i16_area area()const;

		// Return the actor type enum.
		ENUM_ACTOR type()const;

		// Remove this actor instance from actor_manage.
		virtual void erase_actor();

		// Remove the specified actor from actor_manage.
		static void erase_actor(const nguid& aguid);

#pragma region nscript
		// Return whether this actor has an active scripting runtime.
		bool nscript_using()const;

		// Notify the script runtime that DB loading has finished.
		bool nscript_db_loadfinish()const;

		// Push one CSV/DB/NSP dataset into the script runtime.
		template <typename T>
		bool nscript_data_push(const char* asource, const T& adata, bool aedit/* = false*/);

		// Notify the script runtime that one row was deleted.
		template <typename T>
		bool nscript_data_del(int64_t adataid);

		// Read back one script-edited row.
		template <typename T>
		bool nscript_data_checkout(int64_t adataid, T& adata);

		// Read back all script-edited rows for one type.
		template <typename T>
		bool nscript_data_checkout(std::map<int64_t, T>& adata);

		// Return whether one row is marked deleted in the script runtime.
		template <typename T>
		bool nscript_data_checkdel(int64_t adataid);

		// Collect rows marked deleted in the script runtime.
		template <typename T>
		bool nscript_data_checkdel(std::vector<int64_t>& adeldata);

		// Dispatch one actor message into the script runtime.
		template <typename T>
		bool nscript_handle(const T& adata);
#pragma endregion

		// Build a protocol pack from a proto name and JSON payload.
		static std::shared_ptr<pack> jsonpack(const std::string& apbname, const std::string& ajson, i64_actorid aactorid, i64_actorid arequestactorid, bool ahead = false);

		// Build a protocol pack from a strongly typed message.
		template <typename T>
		static std::shared_ptr<pack> net_pack(T& adata, i64_actorid aactorid, i64_actorid arequestactorid, bool ahead = false);

		// Send an already-built pack through a specific session.
		static bool send_pack(i32_sessionid asession, std::shared_ptr<pack>& apack);

		// Serialize and send one message through a specific session.
		template <typename T>
		static bool send(i32_sessionid asession, T& adata, i64_actorid aactorid, i64_actorid arequestactorid);
#pragma region send_server
		// Send one message to a specific server.
		template <typename T>
		static bool send_server(i32_serverid aserverid, T& adata, i64_actorid aactorid, i64_actorid arequestactorid);

		// Send one message to multiple servers.
		template <typename T>
		static bool send_server(const std::set<i32_serverid>& aserverids, T& adata, i64_actorid aactorid, i64_actorid arequestactorid);

		// Send an already-built pack to a specific server.
		static bool send_server(i32_serverid aserverid, std::shared_ptr<pack>& apack);
#pragma endregion

#pragma region kcp
		// Cache which local KCP endpoint index corresponds to a remote server/type pair.
		void kcp_setindex(i32_serverid aserverid, pbnet::ENUM_KCP aenum, int16_t akcpindex);

		// Resolve the cached local KCP endpoint index for a remote server/type pair.
		std::optional<i16_port> kcp_index(i32_serverid aserverid, pbnet::ENUM_KCP aenum);

		// Convenience overload that derives the server id from tid/tcount.
		std::optional<i16_port> kcp_index(int16_t aservertid, int16_t atcount, pbnet::ENUM_KCP aenum);

		// Initiate an outbound KCP connection, primarily used by robot actors.
		bool kcp_connect(i16_port auport, const std::string& aip, i16_port aprot, i64_actorid aactoridserver, std::string& akcpsession)const;

		// Send one already-built packet to a specific actor through UDP/KCP.
		[[nodiscard("check kcp send result")]]
		static bool kcp_sendpack(i64_actorid aactorid, std::shared_ptr<pack>& adata, i16_port auport = 0);

		// Send one already-built packet to multiple actors through UDP/KCP.
		[[nodiscard("check kcp send result")]]
		static bool kcp_sendpack(const std::set<i64_actorid>& aactorids, std::shared_ptr<pack>& adata, i16_port auport = 0);

		// Serialize and send one payload to a specific actor through UDP/KCP.
		template <typename T>
		[[nodiscard("check kcp send result")]]
		static bool kcp_send(i64_actorid aactorid, T& adata, i16_port auport = 0);

		// Serialize and send one payload to multiple actors through UDP/KCP.
		template <typename T>
		[[nodiscard("check kcp send result")]]
		static bool kcp_send(const std::set<i64_actorid>& aactorids, T& adata, i16_port auport = 0);
#pragma endregion

#pragma region send_client
		// Return the node-level routing actor (actor_client or actor_server).
		static i64_actorid actorclient_guid();

		// Send one payload to a set of role actors/clients.
		template <typename T>
		static void send_client(const std::set<i64_actorid>& aids, const T& adata, ENET_PROTOCOL aprotocol = ENET_TCP);

		// Send one payload to one role actor/client.
		template <typename T>
		static void send_client(i64_actorid aid, const T& adata, ENET_PROTOCOL aprotocol = ENET_TCP);

		// Send one payload to a vector of role actors/clients.
		template <typename T>
		static void send_client(const std::vector<i64_actorid>& aids, const T& adata, ENET_PROTOCOL aprotocol = ENET_TCP);

		// Broadcast one payload to every online client.
		template <typename T>
		static void send_client(const T& adata, ENET_PROTOCOL aprotocol = ENET_TCP);

		// Broadcast one payload to all clients in one area.
		template <typename T>
		static void send_clientbyarea(i16_area aarea, const T& adata, ENET_PROTOCOL aprotocol = ENET_TCP);
#pragma endregion

#pragma region send_actor
		// Forward one already-built packet to a concrete actor.
		static void send_actor(const nguid& aguid, const std::shared_ptr<pack>& adata);

		// Broadcast one message to all actors of the given type.
		template <typename T, bool IS_SEND = true>
		static void send_actor(ENUM_ACTOR atype, const std::shared_ptr<T>& adata);

		// Send one message to a concrete actor.
		template <typename T, bool IS_SEND = true>
		static void send_actor(const nguid& aguid, const nguid& arequestguid, const std::shared_ptr<T>& adata);

		// Send one message to a concrete actor with an explicit routing-failure callback.
		template <typename T, bool IS_SEND = true>
		static void send_actor(const nguid& aguid, const nguid& arequestguid, const std::shared_ptr<T>& adata, const std::function<void()>& afailfun);

		// Send one message to a batch of actors.
		template <typename T, bool IS_SEND = true>
		static void send_actor(const std::set<i64_actorid>& asetguid, const nguid& arequestguid, const std::shared_ptr<T>& adata);
#pragma endregion

		// Register a timer task for this actor.
		int64_t set_timer(const np_timerparm& aparm);

		// Return whether this actor participates in the shared broadcast tick.
		bool isbroadcast()const;

		// Enable or disable shared broadcast ticks for this actor.
		void set_broadcast(bool aisbroadcast);

		// Start the shared broadcast timer once for the whole process.
		static void start_broadcast();

		// Debug-print a handled message.
		template <typename T>
		void handle_print(const message<T>& adata)const;

		// Ensure static protocol registration runs once for a derived actor type.
		template <typename TDerived>
		static void first_nregister([[maybe_unused]] ENUM_ACTOR atype);

		// Factory used by generated actor registration code and singleton startup.
		static std::shared_ptr<actor_base> create(ENUM_ACTOR atype, i16_area aarea, i32_actordataid aid, void* aparm = nullptr);
	};

	template <typename T>
	class actor_instance
	{
	public:
		static T& instance();
	};
}//namespace ngl

namespace ngl
{
	template <typename T>
	bool actor_base::nscript_data_push(const char* asource, const T& adata, bool aedit/* = false*/)
	{
		if (!nscript_using())
		{
			return false;
		}
		return nscript_manage::data_push(m_enscript, m_script.get(), tools::type_name<typename T::TDATA>().c_str(), asource, adata, aedit);
	}

	template <typename T>
	bool actor_base::nscript_data_del(int64_t adataid)
	{
		if (!nscript_using())
		{
			return false;
		}
		return nscript_manage::data_del(m_enscript, m_script.get(), tools::type_name<T>().c_str(), adataid);
	}

	template <typename T>
	bool actor_base::nscript_data_checkout(int64_t adataid, T& adata)
	{
		if (!nscript_using())
		{
			return false;
		}
		return nscript_manage::data_checkout(m_enscript, m_script.get(), tools::type_name<T>().c_str(), adataid, adata);
	}

	template <typename T>
	bool actor_base::nscript_data_checkout(std::map<int64_t, T>& adata)
	{
		if (!nscript_using())
		{
			return false;
		}
		return nscript_manage::data_checkout(m_enscript, m_script.get(), tools::type_name<T>().c_str(), adata);
	}

	template <typename T>
	bool actor_base::nscript_data_checkdel(int64_t adataid)
	{
		if (!nscript_using())
		{
			return false;
		}
		return nscript_manage::data_checkdel(m_enscript, m_script.get(), tools::type_name<T>().c_str(), adataid);
	}

	template <typename T>
	bool actor_base::nscript_data_checkdel(std::vector<int64_t>& adeldata)
	{
		if (!nscript_using())
		{
			return false;
		}
		return nscript_manage::data_checkdel(m_enscript, m_script.get(), tools::type_name<T>().c_str(), adeldata);
	}

	template <typename T>
	bool actor_base::nscript_handle(const T& adata)
	{
		if (!nscript_using())
		{
			return false;
		}
		return nscript_manage::handle(m_enscript, m_script.get(), tools::type_name<T>().c_str(), adata);
	}

	template <typename T>
	void actor_base::handle_print(const message<T>& adata)const
	{
		if constexpr (tools::is_protobuf_message<T>::value)
		{
			tools::print_json(*adata.get_data(), true);
		}
		else
		{
			ncjson ltemp;
			njson::push(ltemp, { "message" }, *adata.get_data());
			log_error()->print("{}", ltemp.str());
		}
	}

	template <typename TDerived>
	void actor_base::first_nregister([[maybe_unused]] ENUM_ACTOR atype)
	{
		static std::atomic lfirst = true;
		if (lfirst.exchange(false))
		{
			TDerived::nregister();
		}
	}
}

namespace ngl
{
	template <typename T>
	void tprotocol::tforward::func(int32_t aprotocolnum)
	{
		info* lptemp = tcustoms<true>::func<T>(aprotocolnum);
		if (lptemp != nullptr)
		{
			lptemp->m_toclient[enscript_lua] = [](int64_t aactorid, const char* aoperprotocol, void* aL)->bool
				{
					auto L = (lua_State*)(aL);
					typename T::BASE_TYPE pro;
					if (!ngl::nlua_stack::stack_pop(L, pro))
					{
						return false;
					}
					if (std::string("kcp") == aoperprotocol)
					{
						actor_base::send_client(aactorid, pro, ENET_KCP);
					}
					else if (std::string("tcp") == aoperprotocol)
					{
						actor_base::send_client(aactorid, pro, ENET_TCP);
					}
					return true;
				};
			lptemp->m_toactor[enscript_lua] = [](int64_t aactorid, void* aL)->bool
				{
					auto L = (lua_State*)(aL);
					typename T::BASE_TYPE pro;
					if (!ngl::nlua_stack::stack_pop(L, pro))
					{
						return false;
					}
					actor_base::send_actor(aactorid, nguid::make(), pro);
					return true;
				};
		}
	}

	template <enscript SCRIPT>
	struct tcustoms_send
	{
		template <typename TX>
		static bool send_actor([[maybe_unused]] int64_t aactorid, [[maybe_unused]] void* aL)
		{
			return true;
		}
	};

	template <>
	struct tcustoms_send<enscript_lua>
	{
		template <typename TX>
		static bool send_actor(int64_t aactorid, void* aL)
		{
			auto L = (lua_State*)(aL);
			auto pro = std::make_shared<TX>();
			if (!ngl::nlua_stack::stack_pop(L, *pro))
			{
				return false;
			}
			actor_base::send_actor(aactorid, nguid::make(), pro);
			return true;
		}
	};

	template <bool SCRIPT>
	template <typename T>
	tprotocol::info* tprotocol::tcustoms<SCRIPT>::func(int32_t aprotocolnum /*= -1*/, int32_t ahigh /*= 0*/)
	{
		info* linfo = funcx<T>(aprotocolnum, ahigh);
		if (linfo != nullptr)
		{
			if constexpr (SCRIPT)
			{
				linfo->m_toactor[enscript_lua] = std::bind_front(tcustoms_send<enscript_lua>::send_actor<T>);
			}
		}
		return linfo;
	}
}//namespace ngl

#include "net/nnet.h"
