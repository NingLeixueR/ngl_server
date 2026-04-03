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

#include "actor/protocol/nprotocol_template.h"
#include "actor/generated/pb/net.pb.h"

#include <limits>

namespace ngl
{
	enum E_ACTOR_TIMER
	{
		ET_NULL,
		ET_MONTH,			// Calendar-based monthly timer.
		ET_WEEK,			// Calendar-based weekly timer.
		ET_DAY,				// Daily timer at a fixed hour/minute/second.
		ET_HOUR,			// Hourly timer at a fixed minute/second.
		ET_MIN,				// Minute timer at a fixed second.
		ET_INTERVAL_SEC,	// Fixed-interval timer in seconds.
	};

	struct np_timerparm
	{
		int								m_type = 0;
		int								m_timerid = 0;
		int64_t							m_ms = 0;					// Current wall-clock time in milliseconds.
		int								m_count = 1;				// Remaining trigger count; <= 0 means infinite in some callers.
		std::function<int32_t(int64_t)> m_intervalms = nullptr;		// Computes the next interval after one trigger.
		std::shared_ptr<void>			m_parm = nullptr;			// Optional user payload.
		int64_t							m_triggerms = 0;			// Absolute trigger time of the current firing.

		DPROTOCOL(np_timerparm, m_type, m_timerid, m_ms, m_count, m_triggerms)
	};

	template <typename T>
	void timerparm_set_parm(np_timerparm* apparm, std::shared_ptr<T>& aparm)
	{
		apparm->m_parm = std::static_pointer_cast<void>(aparm);
	}

	// Test
	struct np_robot_pram
	{
		std::string m_cmd;

		DPROTOCOL(np_robot_pram, m_cmd)
	};

	struct nactornode
	{
		std::string		m_name;							// Human-readable server name.
		NODE_TYPE		m_nodetype = NODE_TYPE::FAIL;	// Logical node type.
		i32_serverid	m_serverid = -1;				// Concrete server id.
		std::vector<i16_actortype>	m_actortype;		// Actor types hosted by this server.

		DPROTOCOL(nactornode, m_name, m_serverid, m_actortype)
	};

	struct np_actor_server_register
	{
		DPROTOCOL(np_actor_server_register)
	};

	struct np_connect_actor_server
	{
		int32_t m_serverid;
		int32_t m_session;

		DPROTOCOL(np_connect_actor_server, m_serverid, m_session)
	};

	// actor_client -> actor_server: announce this node and its locally hosted actors.
	struct np_actornode_register
	{
		nactornode m_node;
		std::vector<i64_actorid> m_add;

		DPROTOCOL(np_actornode_register, m_node, m_add)
	};

	// actor_server -> actor_client: return the currently known node list.
	struct np_actornode_register_response
	{
		std::vector<nactornode> m_vec;

		DPROTOCOL(np_actornode_register_response, m_vec)
	};

	// [actor_client -> actor_client]
	// Client connection(actorclient connection,id )
	struct np_actorclient_node_connect
	{
		i32_serverid m_id = -1;

		DPROTOCOL(np_actorclient_node_connect, m_id)
	};

	// Bidirectional node-routing update shared between actor_server and actor_client.
	struct np_actornode_update
	{
		i32_serverid m_id = -1;				// Server that owns the add/del sets.
		std::vector<i64_actorid> m_add;
		std::vector<i64_actorid> m_del;
		std::map<nguid, i32_serverid> m_rolegateway;

		DPROTOCOL(np_actornode_update, m_id, m_add, m_del, m_rolegateway)
	};

	struct np_actornode_update_server
	{
		np_actornode_update m_data;

		DPROTOCOL(np_actornode_update_server, m_data)
	};

	// Toactorclientsynchronizenodeinfo( )
	struct np_actornode_update_mass
	{
		np_actornode_update m_mass;
		std::function<void()> m_fun;

		DPROTOCOL(np_actornode_update_mass_client, m_mass)
	};

	// Deferred task that should execute once actor_client confirms a node connection is live.
	struct np_actornode_connect_task
	{
		i32_serverid m_serverid;
		std::function<void()> m_fun;

		DPROTOCOL(np_actornode_connect_task, m_serverid)
	};

	// [login server -> game server]
	// [login server -> gateway server]
	// Notifyserverplayeraccount through
	struct np_actorrole_login
	{
		std::string		m_session;
		i64_accountid	m_accountid = 0;
		std::string		m_account;
		i64_actorid		m_roleid = 0;
		i32_serverid	m_gameid = 0;
		i32_serverid	m_gatewayid = 0;
		i16_area		m_area = 0;
		bool			m_iscreate = false;
		i32_socket		m_socketid = 0;
		i64_actorid		m_request_actor = 0;

		DPROTOCOL(np_actorrole_login, m_session, m_accountid, m_account, m_roleid, m_gameid, m_gatewayid, m_area, m_iscreate, m_socketid, m_request_actor)
	};

	struct np_gateway_close_session
	{
		i32_actordataid m_roleid;
		i16_area m_area;

		DPROTOCOL(np_gateway_close_session, m_roleid, m_area)
	};

	struct np_actorswitch_process_role
	{
		bool	m_create = false;
		int		m_gatewayid = 0;

		DPROTOCOL(np_actorswitch_process_role, m_gatewayid);
	};

	struct np_actorswitch_process_plays
	{
		std::vector<i64_actorid> m_players;

		DPROTOCOL(np_actorswitch_process_plays, m_players);
	};

	// Server throughthis message connectionsuccessful
	struct np_actorserver_connect
	{
		i32_serverid m_serverid;

		DPROTOCOL(np_actorserver_connect, m_serverid);
	};

	// SESSION connection
	struct np_actor_session_close
	{
		i32_sessionid m_sessionid = 0;

		DPROTOCOL(np_actor_session_close, m_sessionid)
	};

	// [gateway -> game,gateway -> login] 
	// Playerandgateway connection
	struct np_actor_disconnect_close
	{
		i64_actorid m_actorid;

		DPROTOCOL(np_actor_disconnect_close, m_actorid)
	};

	struct np_actor_gatewayid_updata
	{
		bool			m_isremove;
		i64_actorid		m_actorid;
		i32_serverid	m_gatewayid;

		DPROTOCOL(np_actor_gatewayid_updata, m_isremove, m_actorid, m_gatewayid)
	};

	// Synthetic periodic maintenance event broadcast to actors that opted in.
	struct np_actor_broadcast
	{
		DPROTOCOL(np_actor_broadcast)
	};

	// Request a CSV reload using externally supplied file contents.
	struct np_actor_reloadcsv
	{
		std::map<std::string, std::string> m_csvcontent;

		DPROTOCOL(np_actor_reloadcsv, m_csvcontent)
	};

	// Verify CSV versions by file-type hash.
	struct np_actor_csv_verify_version
	{
		std::map<std::string, std::string> m_verify; // key: tab typeid(TAB).hash_code() val:md5

		DPROTOCOL(np_actor_csv_verify_version, m_verify)
	};

	// Senditemtoactor_role
	struct np_actor_senditem
	{
		std::string					m_src;// Itemsource
		std::map<int32_t, int32_t>	m_item;

		DPROTOCOL(np_actor_senditem, m_src, m_item)
	};

	struct chat
	{
		int			m_id = -1;
		i64_actorid m_roleid = -1;
		std::string m_rolename;
		std::string m_content;
		int			m_utc = -1;				// Utc

		DPROTOCOL(chat, m_id, m_roleid, m_rolename, m_content, m_utc)
	};

	struct gateway_socket
	{
		std::string			m_session;
		i16_area			m_area = -1;
		i64_accountid		m_accountid = -1;
		i32_actordataid		m_dataid = -1;
		i32_serverid		m_gameid = -1;
		i32_serverid		m_gatewayid = -1;
		i32_socket			m_socket = -1;

		DPROTOCOL(gateway_socket, m_session, m_area, m_accountid, m_dataid, m_gameid, m_gatewayid, m_socket)
	};

	// Connectiondata
	struct np_actor_gatewayinfo_updata
	{
		std::vector<gateway_socket>	m_add;
		std::vector<i32_socket>		m_delsocket;
		std::vector<i64_actorid>	m_delactorid;

		DPROTOCOL(np_actor_gatewayinfo_updata, m_add, m_delsocket, m_delactorid)
	};

	// Newly addedmail
	struct np_actor_addmail
	{
		i64_actorid					m_roleid = -1;
		int32_t						m_tid = -1;
		std::string					m_parm;
		int32_t						m_dropid = -1;
		int32_t						m_dropcount = -1;

		DPROTOCOL(np_actor_addmail, m_roleid, m_tid, m_parm, m_dropid, m_dropcount)
	};

	struct ncalendar_info
	{
		int32_t m_time = -1;
		int32_t m_calendarid = -1;
		bool m_start = false;

		DPROTOCOL(ncalendar_info, m_time, m_calendarid, m_start)
	};

	// Notify the KCP actor about a newly established KCP session.
	struct np_actor_kcp
	{
		std::string			m_kcpsession;
		i32_session			m_sessionid = -1;
		i64_actorid			m_actoridclient = -1;
		i64_actorid			m_actoridserver = -1;
		std::string			m_uip;
		i16_port			m_uport = std::numeric_limits<i16_port>::max();
		int32_t				m_conv = -1;
		pbnet::ENUM_KCP		m_kcpnum;
		i32_serverid		m_serverid;

		DPROTOCOL(np_actor_kcp, m_kcpsession, m_sessionid, m_actoridclient, m_actoridserver, m_uip, m_uport, m_conv, m_kcpnum, m_serverid)
	};

	struct calendar_utc
	{
		int64_t m_time = -1;
		int32_t m_beg = -1;
		int32_t m_end = -1;

		DPROTOCOL(calendar_utc, m_time, m_beg, m_end)
	};

	// Newly addedmail
	struct np_actor_addnotice
	{
		std::string m_notice;		// Content
		int32_t m_starttime = -1;	// Time
		int32_t m_finishtime = -1;	// Endtime

		DPROTOCOL(np_actor_addnotice, m_notice, m_starttime, m_finishtime)
	};

	// Synthetic close event targeted at a specific actor.
	struct np_actor_close
	{
		DPROTOCOL(np_actor_close)
	};

#ifdef WIN32
# define FindSrcPos(STR) STR.rfind("\\")
#else
# define FindSrcPos(STR) STR.rfind("/")
#endif

	// Serialized log entry forwarded to the log actor.
	struct np_logitem
	{
		int				m_serverid = -1;			// Serverid
		ELOGLEVEL		m_loglevel;					// Loglevel
		ELOG_TYPE		m_type;						// Logtype
		std::string		m_src;						// Log fileposition
		std::string		m_data;						// Logcontent
		int32_t			m_time = -1;				// Log time

		DPROTOCOL(logitem, m_serverid, m_loglevel, m_type, m_src, m_data, m_time)
	};

	struct nactor_logitem
	{
	private:
		/* * Data * */
		std::string				m_src;						// Log fileposition
		ENUM_ACTOR				m_actortype;
		ELOG_TYPE				m_logtype;
		std::source_location	m_source;
	public:
		std::stringstream		m_stream;
		ELOGLEVEL				m_level;
		bool					m_isnet;
		/* * Data * */
		static bool				m_init;
	public:
		
		explicit nactor_logitem(ELOGLEVEL alevel = ELOG_NONE) :
			m_level(alevel),
			m_actortype(ACTOR_NONE),
			m_logtype(ELOG_DEFAULT),
			m_isnet(false)
		{
		}

		explicit nactor_logitem(
			ELOGLEVEL alevel, ENUM_ACTOR aactortype, ELOG_TYPE alogtype, bool aisnet
			, const std::source_location& asource = std::source_location::current()
		) :
			m_level(alevel)
			, m_actortype(aactortype)
			, m_logtype(alogtype)
			, m_isnet(aisnet)
			, m_source(asource)
		{
		}
	private:
		void set_source()
		{
			std::string_view name = m_source.file_name();
			auto pos = FindSrcPos(name);
			if (pos != std::string_view::npos)
			{
				m_src = name.substr(pos + 1);
			}
			else
			{
				m_src = name;
			}
			m_src = std::format("{:^20}:{:^5}", m_src, m_source.line());
		}

		void send(std::shared_ptr<np_logitem> pro, bool aisnet);
	public:
		static bool check_level(ELOGLEVEL alevel)
		{
			return !(alevel < ngl::sysconfig::logconsolelevel() && alevel < ngl::sysconfig::logwritelevel());
		}

		template <typename ...ARGS>
		void print(const std::format_string<ARGS...>& aformat, const ARGS&... aargs)
		{
			if (m_actortype == ENUM_ACTOR::ACTOR_LOG)
			{
				return;
			}
			if (!check_level(m_level))
			{
				return;
			}

			std::string ldata = m_stream.str();
			ldata += std::vformat(aformat.get(), std::make_format_args(aargs...));

			int32_t ltime = (int32_t)tools::time::gettime();
			set_source();

			if (m_level >= ngl::sysconfig::logconsolelevel())
			{
				char ltimebuff[1024];
				tools::time::time2str(ltimebuff, 1024, ltime, "%Y/%m/%d %H:%M:%S");
				logprintf::printf(m_level, m_src.c_str(), ltimebuff, ldata.c_str());
			}

			if (m_init && m_level >= ngl::sysconfig::logwritelevel())
			{
				auto pro = std::make_shared<np_logitem>();
				pro->m_loglevel = m_level;
				pro->m_type = m_logtype;
				pro->m_serverid = nconfig.nodeid();
				pro->m_time = ltime;
				pro->m_src.swap(m_src);
				pro->m_data.swap(ldata);
				send(pro, m_isnet);
			}
		}

		void print(const char* astr)
		{
			print("{}", astr);
		}

		void print(const std::string& astr)
		{
			print("{}", astr);
		}

		template <typename T>
		nactor_logitem& operator<<(const T& adata)
		{
			if (m_init && check_level(m_level))
			{
				m_stream << adata;
			}
			return *this;
		}

		// << Output std::endl
		nactor_logitem& operator<<(std::ostream& (*manipulator)(std::ostream&))
		{
			if (m_init && check_level(m_level))
			{
				m_stream << manipulator;
			}
			return *this;
		}
	};

	// Request login-related summary information for one role.
	struct np_login_request_info
	{
		i64_actorid m_roleid;
		DPROTOCOL(np_login_request_info, m_roleid)
	};

	// actor_example_match->actor_example_manage 
	// Specifiedplayercreatespecified
	struct np_create_example
	{
		pbexample::EPLAY_TYPE m_type;
		std::map<int32_t, i64_actorid> m_roleids;
		DPROTOCOL(np_create_example, m_type, m_roleids)
	};

	// Actor_example_manage->
	struct np_example_entergame_ready
	{
		DPROTOCOL(np_example_entergame_ready)
	};

	// ->Actor_role
	struct np_example_actorid
	{
		pbexample::EPLAY_TYPE m_type;
		i64_actorid m_actorexampleid;
		DPROTOCOL(np_example_id, m_type, m_actorexampleid)
	};

	// ->Actor_example_manage exit
	struct np_example_equit
	{
		pbexample::EPLAY_TYPE m_type;
		i64_actorid m_exampleid;
		DPROTOCOL(np_example_equit, m_type, m_exampleid)
	};

	struct np_thruput_test
	{
		int32_t m_rounds = 0;
		DPROTOCOL(np_thruput_test, m_rounds)
	};

	// /Removetask
	struct np_operator_task
	{
		std::string m_msg;
		bool m_isreceive;
		std::vector<int32_t> m_taskids;

		DPROTOCOL(np_receive_task, m_msg, m_isreceive, m_taskids)
	};

	struct np_operator_task_response
	{
		std::string m_msg;
		bool m_isreceive;

		DPROTOCOL(np_operator_task_response, m_msg, m_isreceive)
	};

	// Fetchrankinfo
	struct np_get_rank
	{
		int32_t m_rankid = 0;

		DPROTOCOL(np_get_rank, m_rankid)
	};

	struct np_get_rank_response
	{
		int32_t m_rankid = 0;
		std::vector<int64_t> m_rolerank;

		DPROTOCOL(np_get_rank_response, m_rankid, m_rolerank)
	};

	// Deleteactivityrank list
	struct np_activityrank_operator
	{
		int32_t m_rankid = 0;
		bool m_iscreate = true;

		DPROTOCOL(np_activityrank_operator, m_rankid, m_iscreate)
	};

	struct np_attribute_remove
	{
		int32_t m_fightid = 0;
		int64_t m_unitid = nguid::make();
		EnumModule m_module = E_ModuleNull;

		DPROTOCOL(np_attribute_remove, m_fightid, m_unitid, m_module)
	};

	struct np_ukcp_waitrecv
	{
		std::string m_recv;
		int32_t m_serverid = 0;
		i64_actorid m_robotid = 0;
		pbnet::ENUM_KCP m_kcpenum;
		i64_actorid m_seractorid = nguid::make();

		DPROTOCOL(np_ukcp_waitrecv, m_recv, m_serverid, m_robotid, m_kcpenum, m_seractorid)
	};
}//namespace ngl
