#pragma once

#include "nactortype.h"
#include "serialize.h"
#include "localtime.h"
#include "serialize.h"
#include "ndefine.h"
#include "nguid.h"
#include "db.pb.h"
#include "pack.h"
#include "type.h"

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

	// 机器人测试
	struct np_robot_pram
	{
		std::vector<std::string> m_parm;

		def_portocol(np_robot_pram, m_parm)
	};

	struct nactornode
	{
		std::string		m_name;// 服务器名称
		i32_serverid	m_serverid;// 服务器id
		std::vector<i16_actortype>	m_actortype;// ENUM_ACTOR_TYPE
		std::string		m_ip;// ip
		i16_port		m_port;// 端口
		def_portocol_function(nactornode, m_name, m_serverid, m_actortype, m_ip, m_port)
	};

	// ---- [actor client -> actor server] register
	// 注册结点
	struct np_actornode_register
	{
		nactornode m_node;
		std::vector<i64_actorid> m_add;
		def_portocol(np_actornode_register, m_node, m_add)
	};

	// ---- [actor client -> actor server] register
	// 注册结点
	struct np_actornode_register_response
	{
		std::vector<nactornode> m_vec;
		def_portocol(np_actornode_register_response, m_vec)
	};

	// ---- [actor client -> actor client] connect(actor客户端相互连接,id大的) 
	// 客户端间相互连接
	struct np_actorclient_node_connect
	{
		i32_serverid m_id;
		def_portocol(np_actorclient_node_connect, m_id)
	};

	// ---- [actor server -> actor client]	[actor client -> actor server] 
	// ---- 服务器向客户端同步结点信息		客户端向服务器同步结点信息
	// 向actor客户端同步结点信息
	struct np_actornode_update
	{
		i32_serverid m_id;// 服务器id
		std::vector<i64_actorid> m_add;
		std::vector<i64_actorid> m_del;
		bool m_actorservermass = true; // 是否给actorclient进行广播
		def_portocol(np_actornode_update, m_id, m_add, m_del, m_actorservermass)
	};

	// 向actor客户端同步结点信息(群发)
	struct np_actornode_update_mass
	{
		np_actornode_update m_mass;
		std::function<void()> m_fun;
		def_portocol(np_actornode_update_mass, m_mass)
	};

	// ---- [local actor client]
	// ---- 连接成功后执行任务
	// 将任务添加到ActorClient中,当指定类型的Actor连接成功后执行该任务
	struct np_actornode_connect_task
	{
		i32_serverid m_serverid;
		std::function<void()> m_fun;
		def_portocol(np_actornode_connect_task, m_serverid)
	};

	// ---- [actor db client -> actor db server]
	// ---- 从db server加载数据
	// 从db server加载数据
	template <EPROTOCOL_TYPE PROTYPE, pbdb::ENUM_DB DBTYPE, typename T>
	struct np_actordb_load
	{
		nguid m_id;
		def_portocol(np_actordb_load, m_id)
	};

	template <EPROTOCOL_TYPE PROTYPE, pbdb::ENUM_DB DBTYPE, typename T>
	struct np_actordb_load_response
	{
		def_portocol(actor_db_load_response<T>)
	};

	template <pbdb::ENUM_DB DBTYPE, typename T>
	struct np_actordb_load_response<EPROTOCOL_TYPE_PROTOCOLBUFF, DBTYPE, T>
	{
		protobuf_data<std::map<nguid, T>>	m_data;
		bool		m_stat = true;
		bool		m_over = true;

		std::map<nguid, T>& data()
		{
			return *m_data.m_data;
		}

		def_portocol(actor_db_load_response<T>, m_stat, m_data, m_over)
	};

	template <pbdb::ENUM_DB DBTYPE, typename T>
	struct np_actordb_load_response<EPROTOCOL_TYPE_CUSTOM, DBTYPE, T>
	{
		std::map<nguid, T>	m_data;
		bool		m_stat = true;
		bool		m_over = true;

		std::map<nguid, T>& data()
		{
			return m_data;
		}

		def_portocol(actor_db_load_response<T>, m_stat, m_data, m_over)
	};

	
	// ---- [actor db server -> actor db client]
	// ---- 保存数据
	// 从db server加载数据
	template <EPROTOCOL_TYPE PROTYPE, pbdb::ENUM_DB DBTYPE, typename T>
	struct np_actordb_save
	{
		np_actordb_save() {}

		def_portocol(actor_db_save<T>)
	};

	template <pbdb::ENUM_DB DBTYPE, typename T>
	struct np_actordb_save<EPROTOCOL_TYPE_PROTOCOLBUFF, DBTYPE, T>
	{
		protobuf_data<std::map<nguid, T>>	m_data;

		np_actordb_save() {}

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

	template <pbdb::ENUM_DB DBTYPE, typename T>
	struct np_actordb_save<EPROTOCOL_TYPE_CUSTOM, DBTYPE, T>
	{
		std::map<nguid, T>	m_data;

		np_actordb_save() {}

		void add(const nguid& akey, const T& avalue)
		{
			m_data.insert(std::make_pair(akey, avalue));
		}

		bool empty()
		{
			return m_data.empty();
		}

		def_portocol(actor_db_save<T>, m_data)
	};

	// 从db server删除数据
	template <EPROTOCOL_TYPE PROTYPE, pbdb::ENUM_DB DBTYPE, typename T>
	struct np_actordb_delete
	{
		std::vector<int64_t> m_data;

		np_actordb_delete() {}

		def_portocol(actor_db_delete<T>, m_data)
	};

	// db cache list  保存缓存列表
	enum enum_cache_list
	{
		enum_clist_save = 1,
		enum_clist_del = 2,
	};
	template <EPROTOCOL_TYPE PROTYPE, typename T>
	struct np_actortime_db_cache
	{
		enum_cache_list			m_type;
		std::set<i64_actorid>	m_ls;

		def_portocol(actor_time_db_cache<T>, m_ls)
	};

	// ---- 模块间转发
	template <typename T>
	struct np_actormodule_forward
	{
	private:
		int64_t				m_identifier = 0;
		std::shared_ptr<T>	m_data;
	public:
		np_actormodule_forward(int64_t aidentifier, std::shared_ptr<T>& adata) :
			m_identifier(aidentifier),
			m_data(adata)
		{}

		np_actormodule_forward(int64_t aidentifier, T& adata) :
			m_identifier(aidentifier),
			m_data(new T(adata))
		{
		}

		np_actormodule_forward(int64_t aidentifier) :
			m_identifier(aidentifier),
			m_data(nullptr)
		{}

		np_actormodule_forward() :
			m_identifier(0),
			m_data(nullptr)
		{}

		T* add_data()
		{
			if (m_data == nullptr)
				m_data = std::shared_ptr<T>(new T());
			//init_protobuf::init(*m_data);
			return m_data.get();
		}

		T* data()
		{
			if (m_data == nullptr)
				return nullptr;
			return m_data.get();
		}

		int64_t identifier()
		{
			return m_identifier;
		}
		
		static const char* name()
		{
			return "actor_module_forward";
		}

		inline bool log(ngl::logserialize& atstr)const
		{
			_va_args_def_portocol_function_log(m_identifier)
				return false;
			if (!atstr('}'))
				return false;
			return true;
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
				m_data = std::shared_ptr<T>(new T());
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
	// ---- 通知服务器玩家账号验证通过
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

		T* get_data()
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

		np_actor_forward() 
		{
		}

		np_actor_forward(np_actor_forward<T, PROTYPE, false, ngl::forward>& adata) :
			m_recvpack(adata.m_recvpack)
		{
		}

		T* get_data()
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

		np_actor_forward() 
		{}

		np_actor_forward(np_actor_forward<T, PROTYPE, true, ngl::forward>& adata) :
			m_recvpack(adata.m_recvpack)
		{}

		T* get_data()
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
		void set_data(std::shared_ptr<T>& adata)
		{
			m_data.m_data = adata;
		}

		T* get_data()
		{
			return m_data.m_data.get();
		}

		np_actor_forward()
		{}

		np_actor_forward(np_actor_forward<T, EPROTOCOL_TYPE_PROTOCOLBUFF, false, T>& adata)
			:m_uid(adata.m_uid), m_area(adata.m_area), m_data(adata.m_data)
		{}

		~np_actor_forward()
		{}

		def_portocol(np_actor_forward,m_uid, m_area, m_data)
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

		T* get_data()
		{
			return m_data.m_data.get();
		}

		void make_data()
		{
			m_data.make();
		}

		np_actor_forward()
		{}

		np_actor_forward(np_actor_forward<T, EPROTOCOL_TYPE_PROTOCOLBUFF, true, T>& adata)
			:m_uid(adata.m_uid), m_area(adata.m_area), m_data(adata.m_data)
		{}

		~np_actor_forward()
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

		T* get_data()
		{
			return m_data != nullptr ? m_data : &m_data_;
		}

		np_actor_forward()
			:m_data(nullptr)
		{}

		np_actor_forward(np_actor_forward<T, EPROTOCOL_TYPE_CUSTOM, ~ISUSING, T>& adata)
			:m_uid(adata.m_uid), m_area(adata.m_area), m_data(adata.m_data), m_data_(adata.m_data_)
		{}

		~np_actor_forward()
		{}

		def_portocol(np_actor_forward, m_uid, m_area, m_data != nullptr ? *m_data : m_data_)
	};

	// ---- actor 切换进程
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

	// ---- 服务器会通过此消息告诉对方连接成功 
	struct np_actorserver_connect
	{
		i32_serverid m_serverid;
		def_portocol(np_actorserver_connect, m_serverid);
	};

	// ---- SESSION断开连接		ACTOR_SESSION_CLOSE
	struct np_actor_session_close
	{
		i32_sessionid m_sessionid = 0;
		def_portocol(np_actor_session_close, m_sessionid)
	};

	// ---- [game,login] 玩家与gateway断开连接  ACTOR_DISCONNECT_CLOSE,
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

	// ---- 日志发送 
	struct logitem
	{
		int				m_serverid = 0;				// 服务器id
		int				m_type;						// ELOG
		std::string		m_head;						// 日志头
		std::string		m_pos;						// 触发日志的文件位置
		std::string		m_str;						// 日志内容
		def_portocol_function(logitem, m_serverid, m_type, m_head, m_pos, m_str)
	};
	struct np_actor_logitem
	{
		logitem m_data;
		def_portocol(np_actor_logitem, m_data)
	};

	// ---- 间隔一段时间发起的全员(所有actor)广播
	// ## 可以在这个广播里推送一些需要处理的任务,例如 保存数据
	struct np_actor_broadcast
	{
		def_portocol(np_actor_broadcast)
	};

	// ---- 重新加载csv
	struct np_actor_reloadcsv
	{
		std::map<std::string, std::string> m_csvcontent; 
		def_portocol(np_actor_reloadcsv, m_csvcontent)
	};

	// ---- 核实csv版本
	struct np_actor_csv_verify_version
	{
		std::map<std::string, int32_t> m_version; // key: tab typeid(TAB).hash_code()
		def_portocol(np_actor_csv_verify_version, m_version)
	};

	// ---- 发送物品给actor role模块
	struct np_actor_senditem
	{
		std::string					m_src;//物品来源
		std::map<int32_t, int32_t>	m_item;
		def_portocol(np_actor_senditem, m_src, m_item)
	};

	struct chat
	{
		int			m_id;
		i64_actorid m_roleid;
		std::string m_rolename;
		std::string m_content;
		int			m_utc;				//发言utc
		def_portocol_function(chat, m_id, m_roleid, m_rolename, m_content, m_utc)
	};

	// ---- role信息同步
	struct np_actor_roleinfo
	{
		protobuf_data<std::vector<pbdb::db_brief>> m_vecinfo;
		def_portocol(np_actor_roleinfo, m_vecinfo)
	};
	
	struct gateway_socket
	{
		std::string			m_session;
		i16_area			m_area;
		i64_accountid		m_accountid;
		i32_actordataid		m_dataid;
		i32_serverid		m_gameid;
		i32_serverid		m_gatewayid;
		i32_socket			m_socket;
		bool				m_iscreate;
		def_portocol_function(gateway_socket, m_session, m_area, m_accountid, m_dataid, m_gameid, m_gatewayid, m_socket, m_iscreate)
	};

	// ---- 更新连接数据
	struct np_actor_gatewayinfo_updata
	{
		std::vector<gateway_socket>	m_add;
		std::vector<i32_socket>		m_delsocket;
		std::vector<i64_actorid>	m_delactorid;
		def_portocol(np_actor_gatewayinfo_updata, m_add, m_delsocket, m_delactorid)
	};

	// ---- 新增邮件
	struct np_actor_addmail
	{
		i64_actorid					m_roleid;
		int32_t						m_tid;
		std::map<int32_t,int32_t>	m_items;
		std::string					m_parm;

		def_portocol(np_actor_addmail, m_roleid, m_tid, m_items, m_parm)
	};

	// ---- 通知actor_manage_activity开启活动
	struct np_actor_openactivity
	{
		i64_actorid m_activityid;
		np_actor_openactivity(i64_actorid aactivityid) :
			m_activityid(aactivityid)
		{}

		np_actor_openactivity() :
			m_activityid(-1)
		{}

		def_portocol(np_actor_openactivity, m_activityid)
	};

	// ---- 通知actor_manage_activity关闭活动
	struct np_actor_closeactivity
	{
		i64_actorid m_activityid;
		bool		m_isnormal;	// 是否正常关闭
		def_portocol(np_actor_closeactivity, m_activityid, m_isnormal)
	};


	// ---- 通知kcp服务器创建连接
	struct np_actor_kcp
	{
		std::string			m_kcpsession;
		i32_session			m_sessionid;
		i16_area			m_area;
		i32_actordataid		m_dataid;
		std::string			m_uip;
		i16_port			m_uport;
		int32_t				m_conv;
		def_portocol(np_actor_kcp, m_kcpsession, m_sessionid, m_area, m_dataid, m_uip, m_uport, m_conv)
	};
}//namespace ngl



