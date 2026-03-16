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

#include "actor/actor_base/nactortype.h"
#include "tools/serialize/nserialize.h"
#include "tools/tab/xml/sysconfig.h"
#include "tools/serialize/ndefine.h"
#include "actor/actor_base/nguid.h"
#include "tools/serialize/pack.h"
#include "actor/pb/example.pb.h"
#include "tools/tab/xml/xml.h"
#include "tools/threadtools.h"
#include "tools/localtime.h"
#include "actor/pb/net.pb.h"
#include "actor/pb/db.pb.h"
#include "tools/tools.h"
#include "tools/type.h"

#include <source_location>
#include <iostream>
#include <sstream>
#include <format>

namespace ngl
{
	// [db client -> db server]
	// Fromdb serverloaddata
	template <pbdb::ENUM_DB DBTYPE, typename T>
	struct np_actordb_load
	{
		nguid m_id = -1;

		DEF_PROTOCOL(m_id)
	};

	enum enum_dbstat
	{
		enum_dbstat_fail = -1,		// Load
		enum_dbstat_success,		// Databasein data,loadsuccessful
		enum_dbstat_create,			// Databasein data, create data
	};

	template <pbdb::ENUM_DB DBTYPE, typename T>
	struct np_actordb_load_response
	{
		nguid				m_id = -1;			// np_actordb_load.m_id 
		std::map<nguid, T>	m_data;
		// If(m_id != nguid::make())m_stat.enum_dbstat_create
		enum_dbstat			m_stat = enum_dbstat_fail;
		bool				m_over = true;

		const std::map<nguid, T>& data()const
		{
			return m_data;
		}

		DEF_PROTOCOL(m_id, m_data, m_stat, m_over)
	};

	// [db client -> db server]
	// Save data
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

	// Deletedata
	template <pbdb::ENUM_DB DBTYPE, typename T>
	struct np_actordb_delete
	{
		std::vector<int64_t> m_data;

		DPROTOCOL(actor_db_delete<T>, m_data)
	};

	// Save datacachelist
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

	// Module forwarding
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
		int32_t m_bufflen = 0;
		const char* m_buff = nullptr;
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
		T m_data;
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
		std::vector<int16_t> m_area;
		std::vector<int32_t> m_uid;
		T m_data;
		ENET_PROTOCOL m_protocol = ENET_TCP;
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
	
	// Datato actor
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


	// Actor switch
	//ACTOR_SPROCESS_ROLE
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
		epb_field_read,		    // Translated comment.
		epb_field_write,	    // Translated comment.
	};

	// Register
	template <typename TDATA>
	struct np_channel_register
	{
		using T = TDATA;
		std::string		m_msg;									// Info
		i64_actorid		m_actorid = nguid::make();				// Nodeid
		// Nodewhether allnode
		bool m_read = false;	// Node [ / ]
		bool m_all = false;		// Whethercan allnode
		// [[ M_all == false under data
		std::set<i64_actorid> m_writeids;						// Whichdata
		std::set<i64_actorid> m_readids;						// Whichdata
		//]]

		std::map<i32_fieldnumber, epb_field> m_field;			// / Whichfield

		DPROTOCOL(np_channel_register, m_msg, m_actorid, m_read, m_all, m_writeids, m_readids, m_field)
	};


	struct nsp_care
	{
		bool m_read = false;
		bool m_all = false;
		std::set<i64_actorid> m_readids;
		std::set<i64_actorid> m_writeids;

		DPROTOCOL(nsp_care, m_read, m_all, m_readids, m_writeids)
	};

	template <typename TDATA>
	struct np_channel_register_reply
	{
		using T = TDATA;
		std::string m_msg;											// Info
		i64_actorid m_actorid;										// Nodeid

		std::set<i64_nodeid> m_nodereadalls;						// Alldata node
		std::set<i64_nodeid> m_nodewritealls;						// Alldata node

		std::map<i64_nodeid, nsp_care> m_care;

		// Node whichfield
		std::map<i16_actortype, std::map<i32_fieldnumber, epb_field>> m_node_fieldnumbers;

		DPROTOCOL(np_channel_register_reply, m_msg, m_actorid, m_nodereadalls, m_nodewritealls, m_care, m_node_fieldnumbers)
	};

	template <typename TDATA>
	struct np_channel_dataid_sync
	{
		using T = TDATA;
		std::string m_msg;										// Info
		i64_actorid m_actorid = 0;								// Nodeid
		bool		m_read = true;								// Node
		bool		m_all = false;								// Node [ / ]alldata
		//if (!m_all)
		//{
		// Partialread-only
		std::set<i64_dataid> m_readpart;
		//}
		//if (!m_all && !m_read)
		//{
		// Partial[ / ]
		std::set<i64_dataid> m_writepart;
		//}

		std::map<i32_fieldnumber, epb_field> m_field;			// / Whichfield

		DPROTOCOL(np_channel_dataid_sync, m_msg, m_actorid, m_read, m_all, m_readpart, m_writepart, m_field)
	};

	template <typename TDATA>
	struct np_channel_exit
	{
		using T = TDATA;
		std::string				m_msg;							// Info
		i64_actorid				m_actorid;
		DPROTOCOL(np_channel_exit, m_msg, m_actorid)
	};

	template <typename TDATA>
	struct np_channel_data
	{
		using T = TDATA;
		std::string m_msg;									// Info
		i64_nodeid m_actorid = 0;							// Data
		bool m_firstsynchronize = false;					// Synchronize
		bool m_recvfinish = false;
		std::map<int64_t, TDATA> m_data;					// 1, Datasynchronize2, data 3, data
		std::vector<int64_t> m_deldata;						// Data delete

		DPROTOCOL(np_channel_data<TDATA>, m_msg, m_actorid, m_firstsynchronize, m_recvfinish, m_data, m_deldata)
	};

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

	// # Getactor stat data
	struct msg_actor_stat
	{
		
		std::vector<msg_actor> m_vec;

		DPROTOCOL(msg_actor_stat, m_vec)
	};
}//namespace ngl