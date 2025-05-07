#pragma once

#include "nprotocol_template.h"

namespace ngl
{
	enum E_ACTOR_TIMER
	{
		ET_NULL,
		ET_MONTH,			// ÿ�´���
		ET_WEEK,			// ÿ�ܴ���
		ET_DAY,				// ÿ�մ���		ahourʱamin��asec��
		ET_HOUR,			// ÿСʱ����	amin��asec��
		ET_MIN,				// ÿ���Ӵ���	asec��
		ET_INTERVAL_SEC,	// ���n�봥��
	};

	struct np_timerparm
	{
		int								m_type = 0;
		int								m_timerid = 0;
		int64_t							m_ms = 0;					// ����ڵ�ǰʱ�� ms
		int								m_count = 1;				// ��������
		std::function<int32_t(int64_t)> m_intervalms = nullptr;		// �������
		std::shared_ptr<void>			m_parm = nullptr;			// �Զ������
		int64_t							m_triggerms = 0;			// ����ʱ�ĺ���

		def_portocol(np_timerparm, m_type, m_timerid, m_ms, m_count, m_triggerms)
	};

	template <typename T>
	void timerparm_set_parm(np_timerparm* apparm, std::shared_ptr<T>& aparm)
	{
		apparm->m_parm = std::static_pointer_cast<void>(aparm);
	}

	struct np_gm
	{
		std::string m_json;

		def_portocol(np_gm, m_json)
	};

	struct np_gm_response
	{
		std::string m_json;

		def_portocol(np_gm_response, m_json)
	};

	// �����˲���
	struct np_robot_pram
	{
		std::vector<std::string> m_parm;

		def_portocol(np_robot_pram, m_parm)
	};

	struct nactornode
	{
		std::string		m_name;						// ����������
		NODE_TYPE		m_nodetype;					// ����������
		i32_serverid	m_serverid = -1;			// ������id
		std::vector<i16_actortype>	m_actortype;	// ENUM_ACTOR_TYPE

		def_portocol_function(nactornode, m_name, m_serverid, m_actortype)
	};

	struct np_actor_server_register
	{
		def_portocol(np_actor_server_register)
	};

	struct np_connect_actor_server
	{
		int32_t m_serverid;
		int32_t m_session;

		def_portocol(np_connect_actor_server, m_serverid, m_session)
	};

	// ---- [actor client -> actor server] register
	// ע����
	struct np_actornode_register
	{
		nactornode m_node;
		std::vector<i64_actorid> m_add;

		def_portocol(np_actornode_register, m_node, m_add)
	};

	// ---- [actor client -> actor server] register
	// ע����
	struct np_actornode_register_response
	{
		std::vector<nactornode> m_vec;

		def_portocol(np_actornode_register_response, m_vec)
	};

	// ---- [actor client -> actor client] connect(actor�ͻ����໥����,id���) 
	// �ͻ��˼��໥����
	struct np_actorclient_node_connect
	{
		i32_serverid m_id = -1;

		def_portocol(np_actorclient_node_connect, m_id)
	};

	// ---- [actor server -> actor client]	[actor client -> actor server] 
	// ---- ��������ͻ���ͬ�������Ϣ		�ͻ����������ͬ�������Ϣ
	struct np_actornode_update
	{
		i32_serverid m_id = -1;				// ������id
		std::vector<i64_actorid> m_add;
		std::vector<i64_actorid> m_del;

		def_portocol(np_actornode_update, m_id, m_add, m_del)
	};

	struct np_actornode_update_server
	{
		np_actornode_update m_data;

		def_portocol(np_actornode_update_server, m_data)
	};

	// ��actor�ͻ���ͬ�������Ϣ(Ⱥ��)
	struct np_actornode_update_mass
	{
		np_actornode_update m_mass;
		std::function<void()> m_fun;

		def_portocol(np_actornode_update_mass_client, m_mass)
	};

	// ---- [local actor client]
	// ---- ���ӳɹ���ִ������
	// ��������ӵ�ActorClient��,��ָ�����͵�Actor���ӳɹ���ִ�и�����
	struct np_actornode_connect_task
	{
		i32_serverid m_serverid;
		std::function<void()> m_fun;

		def_portocol(np_actornode_connect_task, m_serverid)
	};

	// ---- [login -> game]
	// ---- [login -> gateway]
	// ---- ֪ͨ����������˺���֤ͨ��
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

		def_portocol(np_actorrole_login, m_session, m_accountid, m_account, m_roleid, m_gameid, m_gatewayid, m_area, m_iscreate, m_socketid, m_request_actor)
	};

	struct np_gateway_close_session
	{
		i32_actordataid m_roleid;
		i16_area m_area;

		def_portocol(np_gateway_close_session, m_roleid, m_area)
	};

	struct np_actorswitch_process_role
	{
		bool	m_create = false;
		int		m_gatewayid = 0;

		def_portocol(np_actorswitch_process_role, m_create, m_gatewayid);
	};

	struct np_actorswitch_process_plays
	{
		std::vector<i64_actorid> m_players;

		def_portocol(np_actorswitch_process_plays, m_players);
	};

	// ---- ��������ͨ������Ϣ���߶Է����ӳɹ� 
	struct np_actorserver_connect
	{
		i32_serverid m_serverid;

		def_portocol(np_actorserver_connect, m_serverid);
	};

	// ---- SESSION�Ͽ�����		ACTOR_SESSION_CLOSE
	struct np_actor_session_close
	{
		i32_sessionid m_sessionid = 0;

		def_portocol(np_actor_session_close, m_sessionid)
	};

	// ---- [game,login] �����gateway�Ͽ�����  ACTOR_DISCONNECT_CLOSE,
	struct np_actor_disconnect_close
	{
		i64_actorid m_actorid;

		def_portocol(np_actor_disconnect_close, m_actorid)
	};

	struct np_actor_gatewayid_updata
	{
		bool			m_isremove;
		i64_actorid		m_actorid;
		i32_serverid	m_gatewayid;

		def_portocol(np_actor_gatewayid_updata, m_isremove, m_actorid, m_gatewayid)
	};

	// ---- ���һ��ʱ�䷢���ȫԱ(����actor)�㲥
	// ## ����������㲥������һЩ��Ҫ���������,���� ��������
	struct np_actor_broadcast
	{
		def_portocol(np_actor_broadcast)
	};

	// ---- ���¼���csv
	struct np_actor_reloadcsv
	{
		std::map<std::string, std::string> m_csvcontent;

		def_portocol(np_actor_reloadcsv, m_csvcontent)
	};

	// ---- ��ʵcsv�汾
	struct np_actor_csv_verify_version
	{
		std::map<std::string, std::string> m_verify; // key: tab typeid(TAB).hash_code() val:md5

		def_portocol(np_actor_csv_verify_version, m_verify)
	};

	// ---- ������Ʒ��actor roleģ��
	struct np_actor_senditem
	{
		std::string					m_src;//��Ʒ��Դ
		std::map<int32_t, int32_t>	m_item;

		def_portocol(np_actor_senditem, m_src, m_item)
	};

	struct chat
	{
		int			m_id = -1;
		i64_actorid m_roleid = -1;
		std::string m_rolename;
		std::string m_content;
		int			m_utc = -1;				//����utc

		def_portocol_function(chat, m_id, m_roleid, m_rolename, m_content, m_utc)
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
		bool				m_iscreate = false;

		def_portocol_function(gateway_socket, m_session, m_area, m_accountid, m_dataid, m_gameid, m_gatewayid, m_socket, m_iscreate)
	};

	// ---- ������������
	struct np_actor_gatewayinfo_updata
	{
		std::vector<gateway_socket>	m_add;
		std::vector<i32_socket>		m_delsocket;
		std::vector<i64_actorid>	m_delactorid;

		def_portocol(np_actor_gatewayinfo_updata, m_add, m_delsocket, m_delactorid)
	};

	// ---- �����ʼ�
	struct np_actor_addmail
	{
		i64_actorid					m_roleid = -1;
		int32_t						m_tid = -1;
		std::map<int32_t, int32_t>	m_items;
		std::string					m_parm;

		def_portocol(np_actor_addmail, m_roleid, m_tid, m_items, m_parm)
	};

	// ---- ֪ͨactor_manage_activity�����رջ
	struct np_actor_activity
	{
		std::vector<i64_actorid> m_activityids;
		int64_t m_time = -1;
		int32_t m_calendarid = -1;
		bool m_start = false;

		def_portocol(np_actor_openactivity, m_activityids, m_time, m_calendarid, m_start)
	};

	// ---- 
	struct np_actor_task
	{
		std::vector<i64_actorid> m_taskids;
		int64_t m_time = -1;
		int32_t m_calendarid = -1;
		bool m_start = false;

		def_portocol(np_actor_task, m_taskids, m_time, m_calendarid, m_start)
	};

	struct np_actor_calendar_requst
	{
		int64_t m_loginoututc = -1;

		def_portocol(np_actor_calendar_requst, m_loginoututc);
	};

	// ---- ֪ͨkcp��������������
	struct np_actor_kcp
	{
		std::string			m_kcpsession;
		i32_session			m_sessionid = -1;
		i16_area			m_area = -1;
		i32_actordataid		m_dataid = -1;
		std::string			m_uip;
		i16_port			m_uport = -1;
		int32_t				m_conv = -1;

		def_portocol(np_actor_kcp, m_kcpsession, m_sessionid, m_area, m_dataid, m_uip, m_uport, m_conv)
	};

	struct calendar_utc
	{
		int64_t m_time = -1;
		int32_t m_beg = -1;
		int32_t m_end = -1;

		def_portocol_function(calendar_utc, m_time, m_beg, m_end)
	};

	struct np_calendar
	{
		int32_t m_calendarid = -1;
		int64_t m_time = -1;
		bool m_start = false;

		def_portocol(np_calendar, m_calendarid, m_time, m_start)
	};

	// ---- �����ʼ�
	struct np_actor_addnotice
	{
		std::string m_notice;		// ����
		int32_t m_starttime = -1;	// ��ʼʱ��
		int32_t m_finishtime = -1;	// ����ʱ��

		def_portocol(np_actor_addnotice, m_notice, m_starttime, m_finishtime)
	};

	// ---- �ر�ָ��actor(���ܹر�actor_role)
	struct np_actor_close
	{
		def_portocol(np_actor_close)
	};

#ifdef WIN32
# define FindSrcPos(STR) STR.rfind("\\")
#else
# define FindSrcPos(STR) STR.rfind("/")
#endif

	// ---- ��־���� 
	struct np_logitem
	{
		int				m_serverid = -1;			// ������id
		ELOGLEVEL		m_loglevel;					// ��־����
		std::string		m_src;						// ������־���ļ�λ��
		std::string		m_data;						// ��־����
		int32_t			m_time = -1;				// ��־����ʱ��

		def_portocol_function(logitem, m_serverid, m_loglevel, m_src, m_data, m_time)
	};

	struct nactor_logitem
	{
	private:
		/** ��ʱ���� **/
		std::string				m_src;						// ������־���ļ�λ��
		ENUM_ACTOR				m_actortype;
		ELOG_TYPE				m_logtype;
		std::source_location	m_source;
	public:
		std::stringstream		m_stream;
		ELOGLEVEL				m_level;
		/** ��ʱ���� **/
		static bool				m_init;
	public:
		nactor_logitem(ELOGLEVEL alevel = ELOG_NONE) :
			m_actortype(ACTOR_NONE),
			m_logtype(ELOG_LOCAL),
			m_level(alevel)
		{
		}

		nactor_logitem(
			ELOGLEVEL alevel, ENUM_ACTOR aactortype, ELOG_TYPE alogtype
			, const std::source_location& asource = std::source_location::current()
		) :
			m_actortype(aactortype),
			m_logtype(alogtype),
			m_source(asource),
			m_level(alevel)
		{
		}

		~nactor_logitem()
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

		void send(std::shared_ptr<np_logitem> pro);

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
			if (m_init == false)
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

			if (m_level >= ngl::sysconfig::logwritelevel())
			{
				auto pro = std::make_shared<np_logitem>();
				pro->m_loglevel = m_level;
				pro->m_serverid = nconfig::m_nodeid;
				pro->m_time = ltime;
				pro->m_src.swap(m_src);
				pro->m_data.swap(ldata);
				send(pro);
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

		// ���� << ����������� std::endl
		nactor_logitem& operator<<(std::ostream& (*manipulator)(std::ostream&))
		{
			if (m_init && check_level(m_level))
			{
				m_stream << manipulator;
			}
			return *this;
		}
	};

	struct np_channel_check
	{
		int64_t m_timer;
		i16_area m_area;
		def_portocol(np_channel_check, m_timer, m_area)
	};

	struct np_roleban
	{
		std::set<int64_t> m_roleban;
		def_portocol(np_roleban, m_roleban)
	};

	///### ����С��Ϸ
	// ��ҵ�½
	// 1����ȡƥ����Ϣ
	// 2������С��Ϸ��Ϣ
	struct np_login_request_info
	{
		i64_actorid m_roleid;
		def_portocol(np_login_request_info, m_roleid)
	};

	// actor_example_match->actor_example_manage Ϊָ����Ҵ���ָ������С��Ϸ
	struct np_create_example
	{
		pbexample::EPLAY_TYPE m_type;
		std::map<int32_t, i64_actorid> m_roleids;
		def_portocol(np_create_example, m_type, m_roleids)
	};

	// actor_example_manage->����С��Ϸ
	struct np_example_entergame_ready
	{
		def_portocol(np_example_entergame_ready)
	};

	// ����С��Ϸ->actor_role
	struct np_example_actorid
	{
		pbexample::EPLAY_TYPE m_type;
		i64_actorid m_actorexampleid;
		def_portocol(np_example_id, m_type, m_actorexampleid)
	};

	// ����С��Ϸ->actor_example_manage ����С��Ϸ�˳�
	struct np_example_equit
	{
		pbexample::EPLAY_TYPE m_type;
		i64_actorid m_exampleid;
		def_portocol(np_example_equit, m_type, m_exampleid)
	};

	struct np_thruput_test
	{
		int32_t m_rounds = 0;
		def_portocol(np_thruput_test, m_rounds)
	};

}//namespace ngl