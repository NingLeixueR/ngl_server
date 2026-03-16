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

#include "actor/actor_base/nmanage_dbclient.h"
#include "actor/actor_base/handle_pram.h"
#include "actor/actor_base/nactortype.h"
#include "actor/actor_base/nscript.h"
#include "actor/actor_base/ntimer.h"
#include "actor/tab/ttab_servers.h"
#include "actor/actor_base/nguid.h"
#include "net/udp/kcp/ukcp.h"
#include "tools/localtime.h"
#include "tools/ngroup.h"
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
		ENUM_ACTOR		m_type				= nguid::none_type();				// Actortype
		i16_area		m_area				= tab_self_area;					// Area
		i32_actordataid m_id				= nguid::none_actordataid();		// Dataid
		bool			m_manage_dbclient	= false;							// Whether database
		enscript		m_enscript			= enscript_none;					// Scriptsupport
	};

	// # Actor state
	enum actor_stat
	{
		actor_stat_init,  // Initialization stage
		actor_stat_free,  // Idle state
		actor_stat_list,  // - Thread actorqueue
		actor_stat_run,	  // -Online inexecutetask
		actor_stat_close, // Closed state
	};

	// # Message
	template <typename T>
	struct message;

	// # "Ready"component
	enum enum_ready
	{
		e_ready_all		= 0xFFFFFFFF,			// Alldatawhetherloadcomplete
		e_ready_null	= 0x00000000,			// Does not need whetherloadcomplete
		e_ready_db		= 0x00000001,			// Databasedataload
		e_ready_nsp		= 0x00000002,			// Nspdatasynchronize
		e_ready_custom	= 0x00008000,			// Ready

		e_max_custom	= 10,					// Ready
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
	private:
		std::map<int32_t, std::function<bool()>> m_readyfun;
		int32_t m_custom = 0;

		bool check_readybit(int32_t anum, int32_t aready);
	public:
		// # Whetherready
		bool is_ready(int32_t aready = e_ready_all);

		// # Setreadyfunction
		void set_ready(enum_ready aready, const std::function<bool()>& afun);

		// # Set readyfunction
		void set_readybycustom(const std::function<bool()>& afun);
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
		std::map<pbnet::ENUM_KCP, i16_port> m_data;
	};

	class actor_base
	{
		nguid										m_guid = nguid::make();			// actor guid
		std::unique_ptr<nmanage_dbclient>		m_dbclient = nullptr;			// Dbclientcomponentmanage
		bool										m_isload = false;				// Datawhetherloadcomplete
		std::map<pbdb::ENUM_DB, ndb_component*>		m_dbcomponent;					// Dbclientcomponent

		// # Time (allactor)broadcast
		// # Can this broadcast need tohandle task,for example save data
		static int									m_broadcast;					// Broadcast ( )
		static int									m_broadcasttimer;				// Broadcast timerid
		bool										m_isbroadcast = false;			// Whether broadcastmessage
				
		nready										m_ready;						// "Ready"componentinstance
		void*										m_script = nullptr;				// Scripting language support
		enscript									m_enscript = enscript_none;		// Scriptsupport

		std::map<i32_serverid, kcpport>				m_kcpindex;
	public:
		explicit actor_base(const actorparmbase& aparm);

		// # Get"ready"componentinstance
		nready& ready();

		// # Getnmanage_dbclientinstance
		std::unique_ptr<nmanage_dbclient>& manage_dbclient();

		// # Setdb_componentcomponent
		void set_db_component(ndb_component* acomponent);

		// # Initialize dataloadcompleteafter)
		void db_component_init_data();

		// # Initializedb_component
		void init_db_component(bool acreate);

		// # Adddbclient
		void add_dbclient(ndbclient_base* adbclient, i64_actorid aid);

		// # Toactor_dbsenddatarequestafter return
		template <pbdb::ENUM_DB DBTYPE, typename TDBTAB, typename TACTOR>
		bool handle_db(const message<np_actordb_load_response<DBTYPE, TDBTAB>>& adata);

		virtual ~actor_base();

		// # Initialize
		// # Actor function
		// # Let dbclientandactor bind
		virtual void init() {/* Function,if base class */ }

		// # Getactorstate
		virtual actor_stat activity_stat() = 0;

		// # Setactorstate
		virtual void set_activity_stat(actor_stat astat) = 0;

		// # Tasklistwhether
		virtual bool list_empty() = 0;

		// # Task
		virtual void actor_handle(i32_threadid athreadid) = 0;

		// # Addtask
		virtual void push(handle_pram& apram) = 0;

		// # Executehandle after
		virtual void handle_after(handle_pram&) {/* Function,if base class */ }

		// # Actor this function dataloadcompleteafter
		virtual void loaddb_finish(pbdb::ENUM_DB atype, enum_dbstat astat) {/* Function,if base class */ }

		// # Deleteactor
		virtual void release() = 0;

		// # Removeactorbefore
		virtual void erase_actor_before() {/* Function,if base class */ }

		// # Savedbclient
		virtual void save();

		// # Whether singleton
		bool is_single()const;

		// # Get the actor GUID
		const nguid& guid()const;

		// # Get the actor GUID i64_actorid
		i64_actorid id_guid()const;

		// # Get the actor GUID dataid
		i32_actordataid id()const;

		// # Get the actor GUID areaid
		i16_area area()const;

		// # Get the actor GUID actor type
		ENUM_ACTOR type()const;

		// # Removeactor
		virtual void erase_actor();

		// # Removespecifiedactor
		static void erase_actor(const nguid& aguid);

#pragma region nscript
		// # Actorwhether script
		bool nscript_using()const;

		// # Notify the script that DB data loading is complete
		bool nscript_db_loadfinish()const;

		// # Toscript 1, csvdata 2, dbdata 3, nspdata)
		// Parm aname dataname
		// Parm asource datasource(csv,db,nsp)
		// Parm adata data
		// Parm aedit whethercan scriptin
		template <typename T>
		bool nscript_data_push(const char* asource, const T& adata, bool aedit/* = false*/);

		// # Notify the script that the data was deleted
		// Parm aname dataname
		// Parm adataid dataid
		template <typename T>
		bool nscript_data_del(int64_t adataid);

		// # Check whether the data has been modified
		template <typename T>
		bool nscript_data_checkout(int64_t adataid, T& adata);

		// # Check whether a group of data has been modified
		template <typename T>
		bool nscript_data_checkout(std::map<int64_t, T>& adata);

		// # Check whether the data has been deleted
		template <typename T>
		bool nscript_data_checkdel(int64_t adataid);

		// # Check whether a group of data has been deleted
		template <typename T>
		bool nscript_data_checkdel(std::vector<int64_t>& adeldata);

		// # Scripthandlemessage
		template <typename T>
		bool nscript_handle(const T& adata);
#pragma endregion

		// # Build a pack from the proto structure name and JSON message body
		static std::shared_ptr<pack> jsonpack(const std::string& apbname, const std::string& ajson, i64_actorid aactorid, i64_actorid arequestactorid);

		// # Build the pack
		template <typename T>
		static std::shared_ptr<pack> net_pack(T& adata, i64_actorid aactorid, i64_actorid arequestactorid);

		// # Tospecifiedconnectionsenddata
		static bool send_pack(i32_sessionid asession, std::shared_ptr<pack>& apack);

		// # Tospecifiedconnectionsenddata
		template <typename T>
		static bool send(i32_sessionid asession, T& adata, i64_actorid aactorid, i64_actorid arequestactorid);
#pragma region send_server
		// # Senddatatospecifiedserver
		template <typename T>
		static bool send_server(i32_serverid aserverid, T& adata, i64_actorid aactorid, i64_actorid arequestactorid);

		// # Toa group ofserversenddata
		template <typename T>
		static bool send_server(const std::set<i32_serverid>& aserverids, T& adata, i64_actorid aactorid, i64_actorid arequestactorid);

		// # Sendpacktospecifiedserver
		static bool send_server(i32_serverid aserverid, std::shared_ptr<pack>& apack);
#pragma endregion

#pragma region kcp
		void kcp_setindex(i32_serverid aserverid, pbnet::ENUM_KCP aenum, int16_t akcpindex);

		std::optional<i16_port> kcp_index(i32_serverid aserverid, pbnet::ENUM_KCP aenum);

		std::optional<i16_port> kcp_index(int16_t aservertid, int16_t atcount, pbnet::ENUM_KCP aenum);

		// # Kcpconnection
		bool kcp_connect(i16_port auport, const std::string& aip, i16_port aprot, i64_actorid aactoridserver, std::string& akcpsession)const;

		// # Send a pack to the specified actor through UDP/KCP
		static bool kcp_sendpack(i64_actorid aactorid, std::shared_ptr<pack>& adata, i16_port auport = 0);

		// # Send a pack to a group of actors through UDP/KCP
		static bool kcp_sendpack(const std::set<i64_actorid>& aactorids, std::shared_ptr<pack>& adata, i16_port auport = 0);

		// # Send data to the specified actor through UDP/KCP
		template <typename T>
		static bool kcp_send(i64_actorid aactorid, T& adata, i16_port auport = 0);

		// # Send data to a group of actors through UDP/KCP
		template <typename T>
		static bool kcp_send(const std::set<i64_actorid>& aactorids, T& adata, i16_port auport = 0);
#pragma endregion

#pragma region send_client
		// # Get actor (actor_client/actor_server)
		static i64_actorid actorclient_guid();

		// # Send data to a group of clients
		template <typename T>
		static void send_client(const std::set<i64_actorid>& aids, const T& adata, ENET_PROTOCOL aprotocol = ENET_TCP);

		// # Actor_role.guididto clientsenddata
		template <typename T>
		static void send_client(i64_actorid aid, const T& adata, ENET_PROTOCOL aprotocol = ENET_TCP);

		// # Send data to a group of clients
		template <typename T>
		static void send_client(const std::vector<i64_actorid>& aids, const T& adata, ENET_PROTOCOL aprotocol = ENET_TCP);

		// # Send a message to all clients
		template <typename T>
		static void send_client(const T& adata, ENET_PROTOCOL aprotocol = ENET_TCP);

		// # Specifiedareaallclientsendmessage
		template <typename T>
		static void send_clientbyarea(i16_area aarea, const T& adata, ENET_PROTOCOL aprotocol = ENET_TCP);
#pragma endregion

#pragma region send_actor
		// # Sendpacktospecifiedactor
		static void send_actor(const nguid& aguid, const std::shared_ptr<pack>& adata);

		// # SenddatatoallENUM_ACTORtype actor
		template <typename T, bool IS_SEND = true>
		static void send_actor(ENUM_ACTOR atype, const std::shared_ptr<T>& adata);

		// # Send data to the specified actor
		template <typename T, bool IS_SEND = true>
		static void send_actor(const nguid& aguid, const nguid& arequestguid, const std::shared_ptr<T>& adata);

		// # Send data to the specified actor
		template <typename T, bool IS_SEND = true>
		static void send_actor(const nguid& aguid, const nguid& arequestguid, const std::shared_ptr<T>& adata, const std::function<void()>& afailfun);

		// # Send data to a group of actors
		template <typename T, bool IS_SEND = true>
		static void send_actor(const std::set<i64_actorid>& asetguid, const nguid& arequestguid, const std::shared_ptr<T>& adata);
#pragma endregion

		// # Set taskparameters
		int32_t set_timer(const np_timerparm& aparm);

		// # Whethersupportbroadcast
		bool isbroadcast()const;

		// # Setwhethersupportbroadcast
		void set_broadcast(bool aisbroadcast);

		// # Broadcasttimer
		static void start_broadcast();

		// # Protocol
		template <typename T>
		void handle_print(const message<T>& adata)const;

		//# actor_base::create 
		// # Actor automatically
		template <typename TDerived>
		static void first_nregister([[maybe_unused]] ENUM_ACTOR atype);

		// # Used tocreate singletonactor
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
		return nscript_manage::data_push(m_enscript, m_script, tools::type_name<typename T::TDATA>().c_str(), asource, adata, aedit);
	}

	template <typename T>
	bool actor_base::nscript_data_del(int64_t adataid)
	{
		if (!nscript_using())
		{
			return false;
		}
		return nscript_manage::data_del(m_enscript, m_script, tools::type_name<T>().c_str(), adataid);
	}

	template <typename T>
	bool actor_base::nscript_data_checkout(int64_t adataid, T& adata)
	{
		if (!nscript_using())
		{
			return false;
		}
		return nscript_manage::data_checkout(m_enscript, m_script, tools::type_name<T>().c_str(), adataid, adata);
	}

	template <typename T>
	bool actor_base::nscript_data_checkout(std::map<int64_t, T>& adata)
	{
		if (!nscript_using())
		{
			return false;
		}
		return nscript_manage::data_checkout(m_enscript, m_script, tools::type_name<T>().c_str(), adata);
	}

	template <typename T>
	bool actor_base::nscript_data_checkdel(int64_t adataid)
	{
		if (!nscript_using())
		{
			return false;
		}
		return nscript_manage::data_checkdel(m_enscript, m_script, tools::type_name<T>().c_str(), adataid);
	}

	template <typename T>
	bool actor_base::nscript_data_checkdel(std::vector<int64_t>& adeldata)
	{
		if (!nscript_using())
		{
			return false;
		}
		return nscript_manage::data_checkdel(m_enscript, m_script, tools::type_name<T>().c_str(), adeldata);
	}

	template <typename T>
	bool actor_base::nscript_handle(const T& adata)
	{
		if (!nscript_using())
		{
			return false;
		}
		return nscript_manage::handle(m_enscript, m_script, tools::type_name<T>().c_str(), adata);
	}

	template <typename T>
	void actor_base::handle_print(const message<T>& adata)const
	{
		if constexpr (is_protobuf_message<T>::value)
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
	tprotocol::info* tprotocol::tcustoms<SCRIPT>::func(int32_t aprotocolnum /*= -1*/, int8_t ahigh /*= 0*/)
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