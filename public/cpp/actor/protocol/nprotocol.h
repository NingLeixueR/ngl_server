/*
* Copyright (c) [2020-2025] NingLeixueR
* 
* 项目名称：ngl_server
* 项目地址：https://github.com/NingLeixueR/ngl_server
* 
* 本文件是 ngl_server 项目的一部分，遵循 MIT 开源协议发布。
* 您可以按照协议规定自由使用、修改和分发本项目，包括商业用途，
* 但需保留原始版权和许可声明。
* 
* 许可详情参见项目根目录下的 LICENSE 文件：
* https://github.com/NingLeixueR/ngl_server/blob/main/LICENSE
*/
#pragma once

#include "nprotocol_template.h"
#include "net.pb.h"

namespace ngl
{
	enum E_ACTOR_TIMER
	{
		ET_NULL,
		ET_MONTH,			// 每月触发
		ET_WEEK,			// 每周触发
		ET_DAY,				// 每日触发		ahour时amin分asec秒
		ET_HOUR,			// 每小时触发	amin分asec秒
		ET_MIN,				// 每分钟触发	asec秒
		ET_INTERVAL_SEC,	// 间隔n秒触发
	};

	struct np_timerparm
	{
		int								m_type = 0;
		int								m_timerid = 0;
		int64_t							m_ms = 0;					// 相对于当前时间 ms
		int								m_count = 1;				// 触发次数
		std::function<int32_t(int64_t)> m_intervalms = nullptr;		// 触发间隔
		std::shared_ptr<void>			m_parm = nullptr;			// 自定义参数
		int64_t							m_triggerms = 0;			// 触发时的毫秒

		DPROTOCOL(np_timerparm, m_type, m_timerid, m_ms, m_count, m_triggerms)
	};

	template <typename T>
	void timerparm_set_parm(np_timerparm* apparm, std::shared_ptr<T>& aparm)
	{
		apparm->m_parm = std::static_pointer_cast<void>(aparm);
	}

	// 机器人测试
	struct np_robot_pram
	{
		std::vector<std::string> m_parm;

		DPROTOCOL(np_robot_pram, m_parm)
	};

	struct nactornode
	{
		std::string		m_name;							// 服务器名称
		NODE_TYPE		m_nodetype = NODE_TYPE::FAIL;	// 服务器类型
		i32_serverid	m_serverid = -1;				// 服务器id
		std::vector<i16_actortype>	m_actortype;		// ENUM_ACTOR_TYPE

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

	// [actor client -> actor server]
	// 注册结点
	struct np_actornode_register
	{
		nactornode m_node;
		std::vector<i64_actorid> m_add;

		DPROTOCOL(np_actornode_register, m_node, m_add)
	};

	// [actor_client -> actor_server]
	// [回复]注册结点
	struct np_actornode_register_response
	{
		std::vector<nactornode> m_vec;

		DPROTOCOL(np_actornode_register_response, m_vec)
	};

	// [actor_client -> actor_client]
	// 客户端间相互连接(actor客户端相互连接,id大的) 
	struct np_actorclient_node_connect
	{
		i32_serverid m_id = -1;

		DPROTOCOL(np_actorclient_node_connect, m_id)
	};

	// [actor_server -> actor_client]	[actor_client -> actor_server] 
	// 服务器向客户端同步结点信息		客户端向服务器同步结点信息
	struct np_actornode_update
	{
		i32_serverid m_id = -1;				// 服务器id
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

	// 向actor客户端同步结点信息(群发)
	struct np_actornode_update_mass
	{
		np_actornode_update m_mass;
		std::function<void()> m_fun;

		DPROTOCOL(np_actornode_update_mass_client, m_mass)
	};

	// 连接成功后执行任务
	// 将任务添加到actor_client中,当指定类型的Actor连接成功后执行该任务
	struct np_actornode_connect_task
	{
		i32_serverid m_serverid;
		std::function<void()> m_fun;

		DPROTOCOL(np_actornode_connect_task, m_serverid)
	};

	// [login server -> game server]
	// [login server -> gateway server]
	// 通知服务器玩家账号验证通过
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

	// 服务器会通过此消息告诉对方连接成功 
	struct np_actorserver_connect
	{
		i32_serverid m_serverid;

		DPROTOCOL(np_actorserver_connect, m_serverid);
	};

	// SESSION断开连接
	struct np_actor_session_close
	{
		i32_sessionid m_sessionid = 0;

		DPROTOCOL(np_actor_session_close, m_sessionid)
	};

	// [gateway -> game,gateway -> login] 
	// 玩家与gateway断开连接
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

	// 间隔一段时间发起的全员(所有actor)广播
	// 可以在这个广播里推送一些需要处理的任务,例如 保存数据
	struct np_actor_broadcast
	{
		DPROTOCOL(np_actor_broadcast)
	};

	// 重新加载csv
	struct np_actor_reloadcsv
	{
		std::map<std::string, std::string> m_csvcontent;

		DPROTOCOL(np_actor_reloadcsv, m_csvcontent)
	};

	// 核实csv版本
	struct np_actor_csv_verify_version
	{
		std::map<std::string, std::string> m_verify; // key: tab typeid(TAB).hash_code() val:md5

		DPROTOCOL(np_actor_csv_verify_version, m_verify)
	};

	// 发送物品给actor_role
	struct np_actor_senditem
	{
		std::string					m_src;//物品来源
		std::map<int32_t, int32_t>	m_item;

		DPROTOCOL(np_actor_senditem, m_src, m_item)
	};

	struct chat
	{
		int			m_id = -1;
		i64_actorid m_roleid = -1;
		std::string m_rolename;
		std::string m_content;
		int			m_utc = -1;				//发言utc

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

	// 更新连接数据
	struct np_actor_gatewayinfo_updata
	{
		std::vector<gateway_socket>	m_add;
		std::vector<i32_socket>		m_delsocket;
		std::vector<i64_actorid>	m_delactorid;

		DPROTOCOL(np_actor_gatewayinfo_updata, m_add, m_delsocket, m_delactorid)
	};

	// 新增邮件
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

	// 通知kcp服务器创建连接
	struct np_actor_kcp
	{
		std::string			m_kcpsession;
		i32_session			m_sessionid = -1;
		i64_actorid			m_actoridclient = -1;
		i64_actorid			m_actoridserver = -1;
		std::string			m_uip;
		i16_port			m_uport = -1;
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

	// 新增邮件
	struct np_actor_addnotice
	{
		std::string m_notice;		// 内容
		int32_t m_starttime = -1;	// 开始时间
		int32_t m_finishtime = -1;	// 结束时间

		DPROTOCOL(np_actor_addnotice, m_notice, m_starttime, m_finishtime)
	};

	// 关闭指定actor(不能关闭actor_role)
	struct np_actor_close
	{
		DPROTOCOL(np_actor_close)
	};

#ifdef WIN32
# define FindSrcPos(STR) STR.rfind("\\")
#else
# define FindSrcPos(STR) STR.rfind("/")
#endif

	// 日志发送 
	struct np_logitem
	{
		int				m_serverid = -1;			// 服务器id
		ELOGLEVEL		m_loglevel;					// 日志等级
		ELOG_TYPE		m_type;						// 日志类型
		std::string		m_src;						// 触发日志的文件位置
		std::string		m_data;						// 日志内容
		int32_t			m_time = -1;				// 日志发生时间

		DPROTOCOL(logitem, m_serverid, m_loglevel, m_type, m_src, m_data, m_time)
	};

	struct nactor_logitem
	{
	private:
		/** 临时数据 **/
		std::string				m_src;						// 触发日志的文件位置
		ENUM_ACTOR				m_actortype;
		ELOG_TYPE				m_logtype;
		std::source_location	m_source;
	public:
		std::stringstream		m_stream;
		ELOGLEVEL				m_level;
		bool					m_isnet;
		/** 临时数据 **/
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

			int32_t ltime = (int32_t)localtime::gettime();
			set_source();

			if (m_level >= ngl::sysconfig::logconsolelevel())
			{
				char ltimebuff[1024];
				ngl::localtime::time2str(ltimebuff, 1024, ltime, "%Y/%m/%d %H:%M:%S");
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

		// 重载 << 操作符以输出 std::endl
		nactor_logitem& operator<<(std::ostream& (*manipulator)(std::ostream&))
		{
			if (m_init && check_level(m_level))
			{
				m_stream << manipulator;
			}
			return *this;
		}
	};

	///# 例子小游戏
	// 玩家登陆
	// 1、获取匹配信息
	// 2、例子小游戏信息
	struct np_login_request_info
	{
		i64_actorid m_roleid;
		DPROTOCOL(np_login_request_info, m_roleid)
	};

	// actor_example_match->actor_example_manage 
	// 为指定玩家创建指定例子小游戏
	struct np_create_example
	{
		pbexample::EPLAY_TYPE m_type;
		std::map<int32_t, i64_actorid> m_roleids;
		DPROTOCOL(np_create_example, m_type, m_roleids)
	};

	// actor_example_manage->例子小游戏
	struct np_example_entergame_ready
	{
		DPROTOCOL(np_example_entergame_ready)
	};

	// 例子小游戏->actor_role
	struct np_example_actorid
	{
		pbexample::EPLAY_TYPE m_type;
		i64_actorid m_actorexampleid;
		DPROTOCOL(np_example_id, m_type, m_actorexampleid)
	};

	// 例子小游戏->actor_example_manage 例子小游戏退出
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

	// 接收/移除任务
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

	// 拉取排行信息
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

	// 增加删除活动排行榜
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
}//namespace ngl