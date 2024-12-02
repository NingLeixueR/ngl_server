#pragma once

#include "nactortype.h"
#include "serialize.h"
#include "localtime.h"
#include "serialize.h"
#include "sysconfig.h"
#include "logprintf.h"
#include "ndefine.h"
#include "nguid.h"
#include "db.pb.h"
#include "tools.h"
#include "pack.h"
#include "type.h"
#include "xml.h"

#include <source_location>
#include <iostream>
#include <sstream>
#include <format>

namespace ngl
{
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
	// ��actor�ͻ���ͬ�������Ϣ
	struct np_actornode_update
	{
		i32_serverid m_id = -1;				// ������id
		std::vector<i64_actorid> m_add;
		std::vector<i64_actorid> m_del;
		bool m_actorservermass = true;		// �Ƿ��actorclient���й㲥

		def_portocol(np_actornode_update, m_id, m_add, m_del, m_actorservermass)
	};

	// ��actor�ͻ���ͬ�������Ϣ(Ⱥ��)
	struct np_actornode_update_mass
	{
		np_actornode_update m_mass;
		std::function<void()> m_fun;

		def_portocol(np_actornode_update_mass, m_mass)
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

	// ---- [actor db client -> actor db server]
	// ---- ��db server��������
	// ��db server��������
	template <pbdb::ENUM_DB DBTYPE, typename T>
	struct np_actordb_load
	{
		nguid m_id = -1;

		def_portocol(np_actordb_load, m_id)
	};

	template <pbdb::ENUM_DB DBTYPE, typename T>
	struct np_actordb_load_response
	{
		protobuf_data<std::map<nguid, T>>	m_data;
		bool		m_stat = true;
		bool		m_over = true;

		const std::map<nguid, T>& data()const
		{
			return *m_data.m_data;
		}

		def_portocol(actor_db_load_response<T>, m_stat, m_data, m_over)
	};

	// ---- [actor db server -> actor db client]
	// ---- ��������
	// ��db server��������
	template <pbdb::ENUM_DB DBTYPE, typename T>
	struct np_actordb_save
	{
		protobuf_data<std::map<nguid, T>>	m_data;

		void add(const nguid& akey, const T& avalue)
		{
			if (m_data.m_data == nullptr)
				m_data.make();
			m_data.m_data->insert(std::make_pair(akey, avalue));
		}

		bool empty()
		{
			if (m_data.m_data == nullptr)
				return true;
			return m_data.m_data->empty();
		}

		def_portocol(actor_db_save<T>, m_data)
	};

	// ��db serverɾ������
	template <pbdb::ENUM_DB DBTYPE, typename T>
	struct np_actordb_delete
	{
		std::vector<int64_t> m_data;

		def_portocol(actor_db_delete<T>, m_data)
	};

	// db cache list  ���滺���б�
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

		def_portocol(actor_time_db_cache<T>, m_ls)
	};

	// ---- ģ���ת��
	template <typename T>
	struct np_actormodule_forward
	{
	private:
		int64_t				m_identifier = 0;
		std::shared_ptr<T>	m_data;
	public:
		np_actormodule_forward(int64_t aidentifier, const std::shared_ptr<T>& adata) :
			m_identifier(aidentifier),
			m_data(adata)
		{
		}

		np_actormodule_forward(int64_t aidentifier, const T& adata) :
			m_identifier(aidentifier),
			m_data(new T(adata))
		{
		}

		np_actormodule_forward(int64_t aidentifier) :
			m_identifier(aidentifier),
			m_data(nullptr)
		{
		}

		np_actormodule_forward() :
			m_identifier(0),
			m_data(nullptr)
		{
		}

		~np_actormodule_forward()
		{
		}

		T* add_data()
		{
			if (m_data == nullptr)
			{
				m_data = std::make_shared<T>();
			}				
			return m_data.get();
		}

		const T* data()const
		{
			if (m_data == nullptr)
				return nullptr;
			return m_data.get();
		}

		int64_t identifier()const
		{
			return m_identifier;
		}

		static const char* name()
		{
			return "actor_module_forward";
		}

		inline bool push(ngl::serialize& ser)const
		{
			if (ser.push(m_identifier) == false)
				return false;
			if (m_data->SerializeToArray(&ser.buff()[ser.byte()], ser.len() - ser.byte()) == false)
				return false;
			return true;
		}

		inline bool SerializeToArray(void* data, int size) const
		{
			ngl::serialize lserialize((char*)data, size);
			return push(lserialize);
		}

		inline bool pop(ngl::unserialize& ser)
		{
			if (ser.pop(m_identifier) == false)
				return false;
			if (m_data == nullptr)
			{
				m_data = std::make_shared<T>();
			}				
			return m_data->ParseFromArray(&ser.buff()[ser.byte()], ser.len() - ser.byte());
		}

		inline bool ParseFromArray(const void* data, int size)
		{
			ngl::unserialize lunserialize((const char*)data, size);
			return pop(lunserialize);
		}

		inline int bytes(ngl::serialize_bytes& abytes)const
		{
			int lbytes = abytes.bytes(m_identifier);;
			return lbytes += m_data->ByteSize();
		}

		inline int ByteSize()const
		{
			ngl::serialize_bytes lserialize_bytes;
			return bytes(lserialize_bytes);
		}
	};


	template <typename TDATA>
	using mforward = np_actormodule_forward<TDATA>;

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

	template <typename T, EPROTOCOL_TYPE PROTYPE, bool ISUSING, typename TREAL>
	struct np_actor_forward
	{
		using BASE_TYPE = T;
		static const bool isusing = ISUSING;

		const T* get_data()const
		{
			return nullptr;
		}

		std::shared_ptr<T> get_shared()const
		{
			return nullptr;
		}
	};

	template <typename T, EPROTOCOL_TYPE PROTYPE>
	struct np_actor_forward<T, PROTYPE, true, ngl::forward>
	{
		using BASE_TYPE = T;
		static const bool				isusing = true;
		std::vector<i32_actordataid>	m_uid;
		std::vector<i16_area>			m_area;
		std::shared_ptr<pack>			m_recvpack;

		np_actor_forward() = default;

		explicit np_actor_forward(const np_actor_forward<T, PROTYPE, false, ngl::forward>& adata) :
			m_recvpack(adata.m_recvpack)
		{
		}

		const T* get_data()const
		{
			return nullptr;
		}

		std::shared_ptr<T> get_shared()const
		{
			return nullptr;
		}

		def_portocol(np_actor_forward, m_uid, m_area/*, m_data*/)
	};

	template <typename T, EPROTOCOL_TYPE PROTYPE>
	struct np_actor_forward<T, PROTYPE, false, ngl::forward>
	{
		using BASE_TYPE = T;
		static const bool isusing = false;
		std::shared_ptr<pack> m_recvpack;

		np_actor_forward() = default;

		explicit np_actor_forward(const np_actor_forward<T, PROTYPE, true, ngl::forward>& adata) :
			m_recvpack(adata.m_recvpack)
		{}

		const T* get_data()const
		{
			return nullptr;
		}

		std::shared_ptr<T> get_shared()const
		{
			return nullptr;
		}

		def_portocol(np_actor_forward, /*m_uid, m_area,*/ /*m_data*/)
	};


	template <typename T>
	struct np_actor_forward<T, EPROTOCOL_TYPE_PROTOCOLBUFF, true, T>
	{
		using BASE_TYPE = T;
		static const bool				isusing = true;
		std::vector<i32_actordataid>	m_uid;
		std::vector<i16_area>			m_area;
	private:
		protobuf_data<T, true> m_data;
	public:
		void set_data(const std::shared_ptr<T>& adata)
		{
			m_data.m_data = adata;
		}

		const T* get_data()const
		{
			return m_data.m_data.get();
		}

		std::shared_ptr<T> get_shared()const
		{
			return m_data.m_data;
		}

		np_actor_forward() = default;

		explicit np_actor_forward(const np_actor_forward<T, EPROTOCOL_TYPE_PROTOCOLBUFF, false, T>& adata)
			:m_uid(adata.m_uid), m_area(adata.m_area), m_data(adata.m_data)
		{}

		def_portocol(np_actor_forward, m_uid, m_area, m_data)
	};

	template <typename T>
	struct np_actor_forward<T, EPROTOCOL_TYPE_PROTOCOLBUFF, false, T>
	{
		using BASE_TYPE = T;
		static const bool				isusing = false;
		std::vector<i32_actordataid>	m_uid;
		std::vector<i16_area>			m_area;
	private:
		protobuf_data<T, true> m_data;
	public:
		void set_data(std::shared_ptr<T>& adata)
		{
			m_data.m_data = adata;
		}

		const T* get_data()const
		{
			return m_data.m_data.get();
		}

		std::shared_ptr<T> get_shared()const
		{
			return m_data.m_data;
		}

		void make_data()
		{
			m_data.make();
		}

		np_actor_forward() = default;

		explicit np_actor_forward(const np_actor_forward<T, EPROTOCOL_TYPE_PROTOCOLBUFF, true, T>& adata)
			:m_uid(adata.m_uid), m_area(adata.m_area), m_data(adata.m_data)
		{}

		def_portocol(np_actor_forward, m_uid, m_area, m_data)
	};

	template <typename T, bool ISUSING>
	struct np_actor_forward<T, EPROTOCOL_TYPE_CUSTOM, ISUSING, T>
	{
		using BASE_TYPE = T;
		static const bool				isusing = ISUSING;
		std::vector<i32_actordataid>	m_uid;
		std::vector<i16_area>			m_area;
	private:
		T m_data_;
		T* m_data;
	public:
		void set_data(T& adata)
		{
			m_data = &adata;
		}

		const T* get_data()const
		{
			return m_data != nullptr ? m_data : &m_data_;
		}

		np_actor_forward()
			:m_data(nullptr)
		{}

		explicit np_actor_forward(const np_actor_forward<T, EPROTOCOL_TYPE_CUSTOM, ISUSING ? false : true, T>& adata)
			:m_uid(adata.m_uid), m_area(adata.m_area), m_data(adata.m_data), m_data_(adata.m_data_)
		{}

		def_portocol(np_actor_forward, m_uid, m_area, m_data != nullptr ? *m_data : m_data_)
	};

	// ---- actor �л�����
	//ACTOR_SPROCESS_ROLE
	template <typename T>
	struct np_actorswitch_process
	{
		i64_actorid		m_actor = 0;
		i32_serverid	m_serverid = 0;
		i32_serverid	m_toserverid = 0;
		T				m_pram;

		def_portocol(np_actorswitch_process, m_actor, m_serverid, m_toserverid, m_pram);
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
		bool				m_iscreate = -1;

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
	struct logitem
	{
		int				m_serverid = -1;			// ������id
		ELOGLEVEL		m_loglevel;					// ��־����
		std::string		m_src;						// ������־���ļ�λ��
		std::string		m_data;						// ��־����
		int32_t			m_time = -1;				// ��־����ʱ��

		def_portocol_function(logitem, m_serverid, m_loglevel, m_src, m_data, m_time)
	};
	struct np_actor_logitem :public std::enable_shared_from_this<np_actor_logitem>
	{
		logitem m_data;
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
		static bool m_init;
	public:
		np_actor_logitem(ELOGLEVEL alevel = ELOG_NONE):
			m_actortype(ACTOR_NONE),
			m_logtype(ELOG_LOCAL),
			m_level(alevel)
		{
		}

		np_actor_logitem(
			ELOGLEVEL alevel, ENUM_ACTOR aactortype, ELOG_TYPE alogtype
			, const std::source_location& asource = std::source_location::current()
		):
			m_actortype(aactortype),
			m_logtype(alogtype),
			m_source(asource),
			m_level(alevel)
		{}
	private:
		void set_source()
		{
			std::string_view str = m_source.file_name();
			if (auto pos = FindSrcPos(str); pos != std::string_view::npos)
			{
				m_src = str.substr(pos + 1);
			}
			else
			{
				m_src = str;
			}
			m_src = std::format("{:^20}:{:^5}", m_src, m_source.line());
		}

		void send(std::shared_ptr<np_actor_logitem> pro);
	public:
		template <typename ...ARGS>
		void print(const std::format_string<ARGS...>& aformat, const ARGS&... aargs)
		{
			if (m_level >= ngl::sysconfig::loglevel())
			{
				if (sysconfig::logconsole() == false && sysconfig::logiswrite() == false)
					return;
				try
				{
					std::string ldata = m_stream.str();
					ldata += std::vformat(aformat.get(), std::make_format_args(aargs...));
					m_data.m_time = (int32_t)localtime::gettime();
					set_source();
					if (sysconfig::logconsole())
					{
						char ltimebuff[1024];
						ngl::localtime::time2str(ltimebuff, 1024, m_data.m_time, "%Y/%m/%d %H:%M:%S");
						logprintf::printf(m_level, m_src.c_str(), ltimebuff, ldata.c_str());
					}
					if (m_actortype == ENUM_ACTOR::ACTOR_LOG)
						return;
					if (m_init == false || sysconfig::logiswrite() == false)
						return;
					m_data.m_loglevel = m_level;
					m_data.m_serverid = nconfig::m_nodeid;
					m_data.m_src.swap(m_src);
					m_data.m_data.swap(ldata);
					send(shared_from_this());
				}
				catch (...)
				{
					std::cout << "log error!" << std::endl;
				}
			}
		}

		template <typename T>
		np_actor_logitem& operator<<(const T& adata)
		{
			if (m_init && m_level >= ngl::sysconfig::loglevel())
			{
				m_stream << adata;
			}
			return *this;
		}

		// ���� << ����������� std::endl
		np_actor_logitem& operator<<(std::ostream& (*manipulator)(std::ostream&))
		{
			if (m_init && m_level >= ngl::sysconfig::loglevel())
			{
				m_stream << manipulator;
			}
			return *this;
		}

		def_portocol(np_actor_logitem, m_data)
	};

	template <typename TDATA>
	struct np_channel_register
	{
		i64_actorid				m_actorid;
		std::set<i64_actorid>	m_dataid;
		def_portocol(np_channel_register, m_actorid, m_dataid)
	};

	template <typename TDATA>
	struct np_channel_register_reply
	{
		i64_actorid m_actorid;
		def_portocol(np_channel_register_reply, m_actorid)
	};

	template <typename TDATA>
	struct np_channel_data
	{
		protobuf_data<std::map<int64_t, TDATA>> m_data;
		def_portocol(np_channel_data<TDATA>, m_data)
	};

}//namespace ngl



