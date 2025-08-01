﻿#pragma once

#include "ttab_servers.h"
#include "handle_pram.h"
#include "nactortype.h"
#include "localtime.h"
#include "nscript.h"
#include "ntimer.h"
#include "ngroup.h"
#include "nguid.h"
#include "type.h"
#include "impl.h"
#include "ukcp.h"

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
	class actor_manage_dbclient;
	template <typename TDBTAB>
	struct data_modified;

	// # actor的log辅助
	class tools_log
	{
		tools_log(const tools_log&) = delete;
		tools_log& operator=(const tools_log&) = delete;

		actor_base* m_actor;

		std::shared_ptr<nactor_logitem> get_log(const std::source_location& asource, ELOGLEVEL aloglevel, bool anet)const;
	public:
		explicit tools_log(actor_base* aactor = nullptr);

		void set_logactor(actor_base* aactor);

		std::shared_ptr<nactor_logitem> log_debug(const std::source_location& asource = std::source_location::current())const;
		std::shared_ptr<nactor_logitem> log_debug_net(const std::source_location& asource = std::source_location::current())const;

		std::shared_ptr<nactor_logitem> log_info(const std::source_location& asource = std::source_location::current())const;
		std::shared_ptr<nactor_logitem> log_info_net(const std::source_location& asource = std::source_location::current())const;

		std::shared_ptr<nactor_logitem> log_warn(const std::source_location& asource = std::source_location::current())const;
		std::shared_ptr<nactor_logitem> log_warn_net(const std::source_location& asource = std::source_location::current())const;

		std::shared_ptr<nactor_logitem> log_error(const std::source_location& asource = std::source_location::current())const;
		std::shared_ptr<nactor_logitem> log_error_net(const std::source_location& asource = std::source_location::current())const;
	};

	struct actorparmbase
	{
		ENUM_ACTOR		m_type				= nguid::none_type();				// actor类型
		i16_area		m_area				= tab_self_area;					// 区服
		i32_actordataid m_id				= nguid::none_actordataid();		// 数据id
		bool			m_manage_dbclient	= false;							// 是否有数据库依赖
		enscript		m_enscript			= enscript_none;					// 脚本支持
	};

	// # actor的状态
	enum actor_stat
	{
		actor_stat_init,  // 初始化阶段
		actor_stat_free,  // 状态为空闲
		actor_stat_list,  // 繁忙-在等待线程的actor队列中
		actor_stat_run,	  // 繁忙-在线程中执行任务
		actor_stat_close, // 关闭状态
	};

	// # 消息的简单封装
	template <typename T>
	struct message;

	class actor_base : 
		public tools_log
	{
		actor_base() = delete;
		actor_base(const actor_base&) = delete;
		actor_base& operator=(const actor_base&) = delete;
	private:
		struct impl_actor_base;
		struct impl_group;
		impl<impl_actor_base>	m_impl_actor_base;
		ngroup					m_group;
	public:
		explicit actor_base(const actorparmbase& aparm);

#pragma region db
		//# 获取actor_manage_dbclient实例
		using ptr_manage_dbc = std::unique_ptr<actor_manage_dbclient>;
		ptr_manage_dbc& get_actor_manage_dbclient();

		//# 是否需要从数据库加载数据
		bool			isload();

		//# 是否加载完成
		bool			isloadfinish();

		//# 设置db_component组件
		void			set_db_component(ndb_component* acomponent);

		//# 初始化数据(在数据加载完成后)
		void			db_component_init_data();

		//# 初始化db_component
		void			init_db_component(bool acreate);

		//# 添加dbclient
		void			add_dbclient(ndbclient_base* adbclient, i64_actorid aid);

		//# 向actor_db发送数据请求后的返回
		//# DBTYPE 数据类型
		//# TDBTAB 数据表
		//# TACTOR 持有该数据表的actor
		template <pbdb::ENUM_DB DBTYPE, typename TDBTAB, typename TACTOR>
		bool handle(const message<np_actordb_load_response<DBTYPE, TDBTAB>>& adata);
#pragma endregion 

#pragma region virtual_function
		virtual ~actor_base();

		//# 初始化数据
		//# 一般actor对象会在其重载虚函数中
		//# 让dbclient与actor对象进行绑定
		virtual void init() 
		{
		}

		//# 获取actor状态
		virtual actor_stat get_activity_stat() = 0;

		//# 设置actor状态
		virtual void set_activity_stat(actor_stat astat) = 0;

		//# 检查任务列表是否为空
		virtual bool list_empty() = 0;

		//# 进行任务
		virtual void actor_handle(i32_threadid athreadid) = 0;

		//# 添加任务
		virtual void push(handle_pram& apram) = 0;

		//# 执行handle之后调用
		virtual void handle_after(handle_pram&) 
		{
		}

		//# 派生actor重载此函数 会在数据加载完成后调用
		virtual void loaddb_finish(bool adbishave) 
		{
		}

		//# 删除actor时候会被调用
		virtual void release() = 0;

		//# 移除actor前一刻调用
		virtual void erase_actor_before() 
		{
		}
#pragma endregion 

		//# 保存dbclient
		virtual void save();

		//# 是否为单例
		bool is_single();

		//# 获取actor guid
		nguid& guid();

		//# 获取actor guid i64_actorid
		i64_actorid id_guid();

		//# 获取actor guid的数据id
		i32_actordataid id();

		//# 获取actor guid的区服id
		i16_area area();

		//# 获取actor guid的actor type
		ENUM_ACTOR type();

		//# 移除actor自身
		virtual void erase_actor();

		//# 移除指定actor
		static void erase_actor(const nguid& aguid);

		//# 给actor自身添加任务
		void push_task_id(handle_pram& apram);

		//# 向指定actor添加任务
		static void push_task_id(const nguid& aguid, handle_pram& apram);
		static void push_task_id(const std::set<i64_actorid>& asetguid, handle_pram& apram);

		//# 给指定类型的actor添加任务
		static void push_task_type(ENUM_ACTOR atype, handle_pram& apram);

#pragma region nscript
		// # 为nsp_client数据dbnsp设置字符串关联
		void nscript_correlation_dbnsp(const char* aname, const std::function<void(const char*)>& afun);

		template <typename TDBNSP>
		void nscript_correlation_dbnsp(const std::function<void(const char*)>& afun)
		{
			nscript_correlation_dbnsp(tools::type_name<TDBNSP>().c_str(), afun);
		}

		void nscript_change_dbnsp();

		bool nscript_using();

		bool nscript_push_data(const char* adbname, const char* adata_source, const char* adatajson, bool aedit = false);

		template <typename T>
		bool nscript_custom_push_data(const char* adata_source, const T& adata, bool aedit = false)
		{
			std::string ljson;
			tools::custom2json(adata, ljson);
			return nscript_push_data(tools::type_name<T>().c_str(), adata_source, ljson, aedit);
		}

		// # 专属压缩csv数据，所以map.key的类型为int32_t
		template <typename T>
		bool nscript_custom_push_data(const char* adata_source, const std::map<int32_t, T>& adata, bool aedit = false)
		{
			std::string ljson;
			tools::custom2json(adata, ljson);
			return nscript_push_data(tools::type_name<T>().c_str(), adata_source, ljson.c_str(), aedit);
		}

		template <typename T>
		bool nscript_proto_push_data(const char* adata_source, const T& adata, bool aedit = false)
		{
			std::string ljson;
			if (!tools::proto2json(adata, ljson))
			{
				return false;
			}
			return nscript_push_data(tools::type_name<T>().c_str(), adata_source, ljson, aedit);
		}

		// # 专属压缩db数据，所以map.key的类型为int64_t
		template <typename T>
		bool nscript_proto_push_data(const char* adata_source, const std::map<int64_t, T>& adata, bool aedit = false)
		{
			std::string ljson;
			if (!tools::proto2json(adata, ljson))
			{
				return false;
			}
			return nscript_push_data(tools::type_name<T>().c_str(), adata_source, ljson.c_str(), aedit);
		}

		template <typename T>
		bool nscript_proto_push_data(const char* adata_source, const std::map<int64_t, T*>& adata, bool aedit = false)
		{
			std::string ljson;
			if (!tools::proto2json(adata, ljson))
			{
				return false;
			}
			return nscript_push_data(tools::type_name<T>().c_str(), adata_source, ljson.c_str(), aedit);
		}

		// # 压入csv数据表
		template <typename TT>
		bool nscript_push_csv()
		{
			return nscript_custom_push_data<typename TT::type_tab>("csv", TT::instance().tablecsv(), false);
		}

		bool nscript_handle(const char* aname, const char* ajson);

		template <typename T>
		bool nscript_custom_handle(const T& adata)
		{
			std::string ljson;
			json_write ljwrite;
			adata.write(ljwrite);
			ljwrite.get(ljson);
			return nscript_handle(tools::type_name<T>().c_str(), ljson.c_str());
		}

		template <typename T>
		bool nscript_proto_handle(const T& adat)
		{
			std::string ljson;
			if (!tools::proto2json(adat, ljson))
			{
				return false;
			}
			return nscript_handle(tools::type_name<T>().c_str(), ljson.c_str());
		}

		bool nscript_check_outdata(const char* adbname, i64_actorid aactorid, std::string& adatajson);

		template <typename T>
		bool nscript_check_outdata(i64_actorid aactorid, T& adata)
		{
			std::string ljson;
			if (nscript_check_outdata(tools::type_name<T>().c_str(), aactorid, ljson))
			{
				tools::json2proto(ljson, adata);
				return true;
			}
			return false;
		}

		template <typename T>
		bool nscript_check_outdata(std::map<nguid, data_modified<T>>& adata);

		bool nscript_check_outdata_del(const char* adbname, i64_actorid aactorid);

		bool nscript_check_outdata_del(const char* adbname, std::vector<i64_actorid>& aactorid);

		template <typename T>
		bool nscript_check_outdata_del(i64_actorid aactorid);

		template <typename T>
		bool nscript_check_outdata_del(std::vector<i64_actorid>& avec);

		void nscript_db_loadfinish();

#pragma endregion 

#pragma region net
		//# 生成包
		template <typename T>
		static std::shared_ptr<pack> net_pack(T& adata, i64_actorid aactorid, i64_actorid arequestactorid);

		//# 发送数据到指定服务器
		template <typename T>
		static bool send_server(i32_serverid aserverid, T& adata, i64_actorid aactorid, i64_actorid arequestactorid);

		//# 向一组服务器发送数据
		template <typename T, typename CONTAINER>
		static bool send_server(const CONTAINER& aserverids, T& adata, i64_actorid aactorid, i64_actorid arequestactorid);

		//# 发送pack到指定服务器
		template <typename T>
		static bool sendpack_server(i32_serverid aserverid, std::shared_ptr<pack>& apack);

		static std::shared_ptr<pack> jsonpack(const std::string& apbname, const std::string& ajson, i64_actorid aactorid, i64_actorid arequestactorid);

		//# 给指定连接发送数据
		template <typename T>
		static bool sendpack_session(i32_sessionid asession, std::shared_ptr<pack>& apack);

		//# 给指定连接发送数据
		template <typename T>
		static bool send(i32_sessionid asession, T& adata, i64_actorid aactorid, i64_actorid arequestactorid);
#pragma endregion 

#pragma region kcp
		//# 设置udp.kcp session
		void set_kcpssion(i32_session asession);

		//# 获取udp.kcp session
		i32_session get_kcpssion();

		//# 是否支持udp.kcp
		static bool iskcp();

		//# 通过udp.kcp发送数据
		template <typename T>
		bool sendkcp(T& adata, i64_actorid aactorid, int16_t asystemindex = 0);

		//# 通过udp.kcp发送数据
		template <typename T>
		static bool static_sendkcp(i32_sessionid asession, T& adata, i64_actorid aactorid, i64_actorid arequestactorid, int16_t asystemindex = 0);

		//# 通过udp.kcp发送数据
		template <typename T>
		static bool static_sendkcp(const std::vector<i32_sessionid>& asession, T& adata, i64_actorid aactorid, i64_actorid arequestactorid, int16_t asystemindex = 0);

		virtual const char* kcp_session();

		//# 发起kcp连接
		bool connect_kcp(int16_t anum, const std::string& aip, i16_port aprot);
#pragma endregion 

#pragma region send_client
	public:
		template <typename T>
		using tactor_forward = np_actor_forward<T, EPROTOCOL_TYPE_PROTOCOLBUFF, true, T>;
	private:
		static i64_actorid actorclient_guid();

		template <typename T>
		static std::shared_ptr<tactor_forward<T>> create_cpro(const std::shared_ptr<T>& adata)
		{
			auto pro = std::make_shared<tactor_forward<T>>();
			pro->set_data(adata);
			return pro;
		}

		template <typename T>
		static i64_actorid cpro_push_actorid(const std::shared_ptr<tactor_forward<T>>& apro, i64_actorid aid)
		{
			nguid lguid(aid);
			apro->m_uid.push_back(lguid.actordataid());
			apro->m_area.push_back(lguid.area());
			return lguid.make_type(nguid::none_type());
		}
	public:
		//# 根据actor_role.guidid给所在客户端发送数据
		template <typename T>
		static void send_client(i64_actorid aid, const std::shared_ptr<T>& adata)
		{
			if (aid == nguid::make())
			{
				return;
			}
			auto pro = create_cpro(adata);
			cpro_push_actorid(pro, aid);
			handle_pram lpram = handle_pram::create<T>(nguid::make(), nguid::make(), pro);
			push_task_id(actorclient_guid(), lpram);
		}

		template <typename T, typename CONTAINER>
		static void send_client(const CONTAINER& asetid, const std::shared_ptr<T>& adata)
		{
			auto pro = create_cpro(adata);
			for (i64_actorid aactorid : asetid)
			{
				cpro_push_actorid(pro, aactorid);
			}
			handle_pram lpram = handle_pram::create<T>(nguid::make(), nguid::make(), pro);
			push_task_id(actorclient_guid(), lpram);
		}

		//# 向所有客户端发送消息
		template <typename T>
		static void send_client(const std::shared_ptr<T>& adata)
		{
			std::set<i32_serverid>& lgatewayids = sysconfig::gatewayids();
			if (lgatewayids.empty())
			{
				return;
			}
			auto pro = create_cpro(adata);
			cpro_push_actorid(pro, nguid::make());
			send_server(lgatewayids, *pro, nguid::make(), nguid::make());
		}

		//# 往指定区服所有客户端发送消息
		template <typename T>
		static void send_client(i16_area aarea, const std::shared_ptr<T>& adata)
		{
			auto pro = create_cpro(adata);
			cpro_push_actorid(pro, nguid::make());
			ttab_servers::foreach_server(GATEWAY, aarea, [&pro](const tab_servers* atab)
				{
					send_server(atab->m_id, *pro, nguid::make(), nguid::make());
				});
		}
#pragma endregion

#pragma region send_actor
		//# 向指定actor发送pack
		static void send_actor_pack(const nguid& aguid, const std::shared_ptr<pack>& adata)
		{
			handle_pram lpram = handle_pram::create_pack(aguid, nguid::make(), adata);
			push_task_id(aguid, lpram);
		}

		template <typename T, bool IS_SEND = true>
		static void send_actor(ENUM_ACTOR atype, const std::shared_ptr<T>& adata)
		{
			handle_pram lpram = handle_pram::create<T, IS_SEND>(nguid::make_self(atype), nguid::make(), adata);
			lpram.m_forwardtype = true;
			push_task_type(atype, lpram);
		}

		//# 发送数据到指定的actor
		template <typename T, bool IS_SEND = true>
		static void send_actor(
			const nguid& aguid, const nguid& arequestguid, const std::shared_ptr<T>& adata
		)
		{
			handle_pram lpram = handle_pram::create<T, IS_SEND>(aguid, arequestguid, adata);
			push_task_id(aguid, lpram);
		}

		//# 发送数据到指定的actor
		template <typename T, bool IS_SEND = true>
		static void send_actor(
			const nguid& aguid, const nguid& arequestguid, const std::shared_ptr<T>& adata, const std::function<void()>& afailfun
		)
		{
			handle_pram lpram = handle_pram::create<T, IS_SEND>(aguid, arequestguid, adata, afailfun);
			push_task_id(aguid, lpram);
		}

		template <typename T, bool IS_SEND = true>
		static void send_actor(const std::set<i64_actorid>& asetguid, const nguid& arequestguid, const std::shared_ptr<T>& adata)
		{
			if (!asetguid.empty())
			{
				handle_pram lpram = handle_pram::create<T, IS_SEND>(asetguid, arequestguid, adata);
				push_task_id(asetguid, lpram);
			}
		}

#pragma endregion

		// # 方便调试打印协议
		template <bool PROTO, typename T>
		void handle_print(const message<T>& adata)
		{
			if constexpr (PROTO)
			{
				tools::print_json2proto(*adata.get_data(), true);
			}
			else
			{
				json_write ljson;
				ljson.write("message", *adata.get_data());
				std::string lstr;
				ljson.get(lstr);
				log_error()->print("{}", lstr);
			}
		}

#pragma region group
		//# 创建一个群发分组(可以指定ActorType,主要是为了区分客户端与普通actor)
		int32_t create_group(ENUM_ACTOR atype = ACTOR_NONE);
		
		//# 移除一个分组
		void remove_group(int32_t agroupid);
		
		//# 将成员加入某个群发分组
		bool add_group_member(int32_t agroupid, i64_actorid amember);
		
		//# 将成员从某个群发分组中移除
		void remove_group_member(int32_t agroupid, i64_actorid amember);
		
		//# 获取group id中的actor列表与类型
		const std::set<i64_actorid>* get_group(int32_t agroupid);

		//# 给一组成员发送消息
		template <typename T>
		bool send_group(int agroupid, std::shared_ptr<T>& adata)
		{
			std::set<i64_actorid>* lset = get_group(agroupid);
			if(lset == nullptr)
			{
				return false;
			}
			std::set<i64_actorid> lclient;
			std::set<i64_actorid> lactors;
			for (i64_actorid aactor : *lset)
			{
				if ((ENUM_ACTOR)nguid::type(aactor) != ACTOR_ROBOT)
				{
					lactors.insert(aactor);
				}
				else
				{
					lclient.insert(aactor);
				}
			}
			if (!lclient.empty())
			{
				send_client(lclient, adata);
			}
			if (!lactors.empty())
			{
				send_actor(lactors, adata);
			}
			return true;
		}
#pragma endregion 
		
#pragma region broadcast
		//# 设置定时任务参数
		int32_t set_timer(const np_timerparm& aparm);

		//# 是否支持广播
		bool isbroadcast();

		//# 设置是否支持广播
		void set_broadcast(bool aisbroadcast);

		//# 启动广播定时器
		static void start_broadcast();
#pragma endregion 

		static void create_log(ENUM_ACTOR atype)
		{
			if (atype != ngl::ACTOR_LOG)
			{
				nlogactor lnlogactor(atype, ngl::ELOG_LOCAL);
				ngl::actor_base::create(ngl::ACTOR_LOG, tab_self_area, lnlogactor.m_value32);
			}
		}

		//# actor_base::create 
		//# 构造actor对象会自动被调用
		template <typename TDerived>
		static void first_nregister(ENUM_ACTOR atype)
		{
			static bool lfirst = true;
			if (lfirst == false)
			{
				return;
			}
			lfirst = false;
			TDerived::nregister();
			create_log(atype);
		}

		//# 用于创建非单例actor
		static std::shared_ptr<actor_base> create(ENUM_ACTOR atype, i16_area aarea, i32_actordataid aid, void* aparm = nullptr);
	};

	template <typename T>
	class actor_instance
	{
	public:
		// 定义在 actor_manage.h
		static T& instance();
	};
}//namespace ngl

namespace ngl
{
	template <typename T>
	void tprotocol::tforward::func(int32_t aprotocolnum)
	{
		pinfo* lptemp = tcustoms<EPROTOCOL_TYPE_PROTOCOLBUFF,true>::func<T>(aprotocolnum);
		if (lptemp != nullptr)
		{
			lptemp->m_forward = true;
			lptemp->m_toclient = [](int64_t aactorid, const char* adata)
				{
					auto pro = std::make_shared<typename T::BASE_TYPE>();
					if (!tools::json2proto<typename T::BASE_TYPE>(adata, *pro))
					{
						return false;
					}
					actor_base::send_client(aactorid, pro);
					return true;
				};
			lptemp->m_toactor = [](int64_t aactorid, const char* adata)
				{
					auto pro = std::make_shared<typename T::BASE_TYPE>();
					if (!tools::json2proto<typename T::BASE_TYPE>(adata, *pro))
					{
						return false;
					}
					actor_base::send_actor(aactorid, nguid::make(), pro);
					return true;
				};
		}
	}

	template <EPROTOCOL_TYPE TYPE, bool REGISTER_JSON>
	template <typename TX>
	void tprotocol::tcustoms<TYPE, REGISTER_JSON>::send_actor(int64_t aactorid, const char* adata)
	{
		ngl::json_read lread(adata);
		auto pro = std::make_shared<TX>();
		pro->read(lread);
		actor_base::send_actor(aactorid, nguid::make(), pro);
	}
}