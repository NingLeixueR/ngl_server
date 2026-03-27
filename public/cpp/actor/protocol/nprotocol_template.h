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
// File overview: Declares interfaces for protocol.

#pragma once

#include "actor/actor_base/core/nactortype.h"
#include "tools/serialize/nserialize.h"
#include "tools/tab/xml/sysconfig.h"
#include "tools/serialize/ndefine.h"
#include "actor/actor_base/core/nguid.h"
#include "tools/serialize/pack.h"
#include "actor/generated/pb/example.pb.h"
#include "tools/tab/xml/xml.h"
#include "tools/tools/tools_thread.h"
#include "tools/tools/tools_time.h"
#include "actor/generated/pb/net.pb.h"
#include "actor/generated/pb/db.pb.h"
#include "tools/tools.h"
#include "tools/type.h"

#include <source_location>
#include <iostream>
#include <sstream>
#include <format>

namespace ngl
{
	// Request one DB table or one DB row from the DB actor.
	template <pbdb::ENUM_DB DBTYPE, typename T>
	struct np_actordb_load
	{
		nguid m_id = -1;

		DEF_PROTOCOL(m_id)
	};

	enum enum_dbstat
	{
		enum_dbstat_fail = -1,		// Load failed.
		enum_dbstat_success,		// Existing DB rows were loaded successfully.
		enum_dbstat_create,			// No row existed, so a new default row was created.
	};

	template <pbdb::ENUM_DB DBTYPE, typename T>
	struct np_actordb_load_response
	{
		nguid				m_id = -1;			// Mirrors np_actordb_load::m_id.
		std::map<nguid, T>	m_data;
		// For single-row loads, enum_dbstat_create means the row had to be created.
		enum_dbstat			m_stat = enum_dbstat_fail;
		bool				m_over = true;		// Final chunk flag for streamed responses.

		const std::map<nguid, T>& data()const
		{
			return m_data;
		}

		DEF_PROTOCOL(m_id, m_data, m_stat, m_over)
	};

	// Batch-save one DB table's dirty rows.
	template <pbdb::ENUM_DB DBTYPE, typename T>
	struct np_actordb_save
	{
		std::map<nguid, T > m_data;

		void add(const nguid& akey, const T& avalue)
		{
			m_data.insert(std::make_pair(akey, avalue));
		}

		bool empty()
		{
			return m_data.empty();
		}

		DEF_PROTOCOL(m_data)
	};

	// Batch-delete rows from one DB table.
	template <pbdb::ENUM_DB DBTYPE, typename T>
	struct np_actordb_delete
	{
		std::vector<int64_t> m_data;

		DPROTOCOL(actor_db_delete<T>, m_data)
	};

	// Deferred DB cache maintenance operation.
	enum enum_cache_list
	{
		enum_clist_save = 1,
		enum_clist_del = 2,
	};
	template <typename T>
	struct np_actortime_db_cache
	{
		enum_cache_list			m_type;
		std::set<i64_actorid>	m_ls;

		DPROTOCOL(actor_time_db_cache<T>, m_ls)
	};

	// Wrap a payload with an identifier so module-level routing can target one logical sub-object.
	template <typename T>
	struct np_actormodule_forward
	{
	private:
		int64_t				m_identifier = 0;
		std::shared_ptr<T>	m_data;
	public:
		using BASE_TYPE = T;

		DEF_PROTOCOL(m_identifier, *m_data)
		DEF_NLUA_SPECIAL_FUNCTION({ "m_identifier", "m_data" }, m_identifier, *m_data)

		np_actormodule_forward(int64_t aidentifier, const std::shared_ptr<T>& adata) :
			m_identifier(aidentifier),
			m_data(adata)
		{
		}

		np_actormodule_forward(int64_t aidentifier, const T& adata) :
			m_identifier(aidentifier),
			m_data(std::make_shared<T>(adata))
		{
		}

		explicit np_actormodule_forward(int64_t aidentifier) :
			m_identifier(aidentifier),
			m_data(std::make_shared<T>())
		{
		}

		np_actormodule_forward() :
			m_identifier(0),
			m_data(std::make_shared<T>())
		{
		}

		T* data()const
		{
			return m_data.get();
		}

		int64_t identifier()const
		{
			return m_identifier;
		}

		void set_identifier(int64_t aidentifier)
		{
			m_identifier = aidentifier;
		}
	};

	template <typename TDATA>
	using mforward = np_actormodule_forward<TDATA>;

	struct forward
	{
		int32_t m_bufflen = 0;   // Serialized payload size.
		const char* m_buff = nullptr; // Borrowed view into the serialized payload.
	};

	namespace ser
	{
		template <>
		struct serialize_format<forward>
		{
			static bool push(serialize_push* aserialize, const forward& adata)
			{
				if (!serialize_format<int32_t>::push(aserialize, adata.m_bufflen))
				{
					return false;
				}
				return aserialize->basetype((void*)adata.m_buff, adata.m_bufflen);
			}

			static bool pop(serialize_pop* aserialize, forward& adata)
			{
				if (!serialize_format<int32_t>::pop(aserialize, adata.m_bufflen))
				{
					return false;
				}
				if (adata.m_bufflen > (aserialize->len() - aserialize->pos()))
				{
					return false;
				}
				adata.m_buff = &aserialize->buff()[aserialize->pos()];
				return true;
			}

			static void bytes(serialize_byte* aserialize, const forward& adata)
			{
				serialize_format<int32_t>::bytes(aserialize, adata.m_bufflen);
				aserialize->move_pos(adata.m_bufflen);
			}
		};
	}//namespace ser

	template <typename T>
	struct forward_c2g
	{
		T m_data; // Original client payload wrapped for gateway forwarding.
	};

	namespace ser
	{
		template <typename T>
		struct serialize_format<forward_c2g<T>>
		{
			static bool push(serialize_push* aserialize, const forward_c2g<T>& adata)
			{
				return ngl::ser::nserialize::push(aserialize, adata.m_data);
			}

			static bool pop(serialize_pop* aserialize, forward_c2g<T>& adata)
			{
				return ngl::ser::nserialize::pop(aserialize, adata.m_data);
			}

			static void bytes(serialize_byte* aserialize, const forward_c2g<T>& adata)
			{
				ngl::ser::nserialize::bytes(aserialize, adata.m_data);
			}
		};
	}//namespace ser

	template <typename T>
	struct forward_g2c
	{
		std::vector<int16_t> m_area;       // Target client areas.
		std::vector<int32_t> m_uid;        // Target client actor ids within those areas.
		T m_data;                          // Original server payload.
		ENET_PROTOCOL m_protocol = ENET_TCP; // Preferred transport to the client.
	};

	namespace ser
	{
		template <typename T>
		struct serialize_format<forward_g2c<T>>
		{
			static bool push(serialize_push* aserialize, const forward_g2c<T>& adata)
			{
				return ngl::ser::nserialize::push(aserialize, adata.m_area, adata.m_uid, adata.m_protocol, adata.m_data);
			}

			static bool pop(serialize_pop* aserialize, forward_g2c<T>& adata)
			{
				return ngl::ser::nserialize::pop(aserialize, adata.m_area, adata.m_uid, adata.m_protocol, adata.m_data);
			}

			static void bytes(serialize_byte* aserialize, const forward_g2c<T>& adata)
			{
				ngl::ser::nserialize::bytes(aserialize, adata.m_area, adata.m_uid, adata.m_protocol, adata.m_data);
			}
		};
	}//namespace ser

	template <typename T, typename Y>
	struct np_actor_forward
	{
		Y m_data;

		DPROTOCOL(np_actor_forward, m_data)
	};
	
	// Synthetic wrapper used to fan one payload out to many actors.
	template <typename T>
	struct np_mass_actor
	{
		std::set<i64_actorid> m_actorids;
	private:
		std::shared_ptr<T> m_data;
	public:
		np_mass_actor() :
			m_data(std::make_shared<T>())
		{}

		explicit np_mass_actor(const std::shared_ptr<T>& adata) :
			m_data(adata)
		{}

		T* data()
		{
			return m_data.get();
		}

		std::shared_ptr<T> shared_data()
		{
			return m_data;
		}

		void set_actorids(std::set<i64_actorid>& aactorids)
		{
			m_actorids = aactorids;
		}

		void add_actorid(i64_actorid aactorid)
		{
			m_actorids.insert(aactorid);
		}

		DEF_JSONFUNCTION_SPECIAL({ "m_actorids","m_data" }, m_actorids, *m_data)
		DEF_PROTOCOL(m_actorids, *m_data)
		DEF_NLUA_SPECIAL_FUNCTION({ "m_actorids", "m_data" }, m_actorids, *m_data)
	};


	// Request that one actor/process migration step move an actor to another server.
	template <typename T>
	struct np_actorswitch_process
	{
		i64_actorid		m_actor = 0;
		i32_serverid	m_serverid = 0;
		i32_serverid	m_toserverid = 0;
		T				m_pram;

		DPROTOCOL(np_actorswitch_process, m_actor, m_serverid, m_toserverid, m_pram);
	};

	enum epb_field
	{
		epb_field_read,		    // Field may be mirrored to the subscriber.
		epb_field_write,	    // Field may be written back by the subscriber.
	};

	// NSP registration request describing row scope and field scope.
	template <typename TDATA>
	struct np_channel_register
	{
		using T = TDATA;
		std::string		m_msg;									// Info
		i64_actorid		m_actorid = nguid::make();				// Peer node/actor id.
		bool m_read = false;									// True for read-only peers, false for read/write peers.
		bool m_all = false;										// True when the peer subscribes to all rows.
		std::set<i64_actorid> m_writeids;						// Writable rows when m_all == false and m_read == false.
		std::set<i64_actorid> m_readids;						// Readable rows when m_all == false.

		std::map<i32_fieldnumber, epb_field> m_field;			// Per-field read/write permissions.

		DPROTOCOL(np_channel_register, m_msg, m_actorid, m_read, m_all, m_writeids, m_readids, m_field)
	};


	struct nsp_care
	{
		bool m_read = false;					// Read-only or read/write subscription.
		bool m_all = false;						// Applies to all rows when true.
		std::set<i64_actorid> m_readids;		// Explicit readable rows when m_all == false.
		std::set<i64_actorid> m_writeids;		// Explicit writable rows when m_all == false and !m_read.

		DPROTOCOL(nsp_care, m_read, m_all, m_readids, m_writeids)
	};

	// NSP registration reply containing known peer scopes and merged field rules.
	template <typename TDATA>
	struct np_channel_register_reply
	{
		using T = TDATA;
		std::string m_msg;											// Info
		i64_actorid m_actorid;										// Nodeid

		std::set<i64_nodeid> m_nodereadalls;						// Peers that read all rows.
		std::set<i64_nodeid> m_nodewritealls;						// Peers that write all rows.

		std::map<i64_nodeid, nsp_care> m_care;

		// Merged actor-type -> field-permission table known by the NSP server.
		std::map<i16_actortype, std::map<i32_fieldnumber, epb_field>> m_node_fieldnumbers;

		DPROTOCOL(np_channel_register_reply, m_msg, m_actorid, m_nodereadalls, m_nodewritealls, m_care, m_node_fieldnumbers)
	};

	// NSP incremental peer-scope announcement.
	template <typename TDATA>
	struct np_channel_dataid_sync
	{
		using T = TDATA;
		std::string m_msg;										// Info
		i64_actorid m_actorid = 0;								// Peer node/actor id.
		bool		m_read = true;								// Read-only or read/write peer.
		bool		m_all = false;								// True when the peer covers all rows.
		std::set<i64_dataid> m_readpart;						// Explicit readable rows when m_all == false.
		std::set<i64_dataid> m_writepart;						// Explicit writable rows when m_all == false and !m_read.

		std::map<i32_fieldnumber, epb_field> m_field;			// Per-field read/write permissions.

		DPROTOCOL(np_channel_dataid_sync, m_msg, m_actorid, m_read, m_all, m_readpart, m_writepart, m_field)
	};

	// NSP peer-exit notification.
	template <typename TDATA>
	struct np_channel_exit
	{
		using T = TDATA;
		std::string				m_msg;							// Info
		i64_actorid				m_actorid;
		DPROTOCOL(np_channel_exit, m_msg, m_actorid)
	};

	// NSP data packet used for both initial full sync and incremental updates.
	template <typename TDATA>
	struct np_channel_data
	{
		using T = TDATA;
		std::string m_msg;									// Info
		i64_nodeid m_actorid = 0;							// Source peer actor id.
		bool m_firstsynchronize = false;					// True while streaming the initial full snapshot.
		bool m_recvfinish = false;							// Final chunk flag for streamed snapshots.
		std::map<int64_t, TDATA> m_data;					// Created/updated rows.
		std::vector<int64_t> m_deldata;						// Deleted row ids.

		DPROTOCOL(np_channel_data<TDATA>, m_msg, m_actorid, m_firstsynchronize, m_recvfinish, m_data, m_deldata)
	};

	// Periodic registration retry tick sent to local NSP helpers.
	template <typename TDATA>
	struct np_channel_check
	{
		using T = TDATA;
		std::string m_msg;									// Info
		int64_t m_timer;
		i16_area m_area;
		DPROTOCOL(np_channel_check, m_msg, m_timer, m_area)
	};

	struct np_gm
	{
		std::string m_json;

		DPROTOCOL(np_gm, m_json)
	};

	struct np_gm_response
	{
		std::string m_json;

		DPROTOCOL(np_gm_response, m_json)
	};

	struct np_testlua
	{
		std::string m_name;
		std::map<int, std::string> m_data;

		DPROTOCOL(np_testlua, m_name, m_data)
	};

	struct msg_actor
	{
		std::string m_actor_name;
		std::map<i16_area, std::vector<i32_actordataid>> m_actor;

		DPROTOCOL(msg_actor, m_actor_name, m_actor)
	};

	// Aggregated actor statistics returned to monitoring / GM callers.
	struct msg_actor_stat
	{
		
		std::vector<msg_actor> m_vec;

		DPROTOCOL(msg_actor_stat, m_vec)
	};
}//namespace ngl
