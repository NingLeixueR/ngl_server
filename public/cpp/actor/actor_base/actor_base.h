#pragma once

#include "ttab_servers.h"
#include "handle_pram.h"
#include "nactortype.h"
#include "localtime.h"
#include "ntimer.h"
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

	struct actorparmbase
	{
		ENUM_ACTOR		m_type				= nguid::none_type();				// actor类型
		i16_area		m_area				= tab_self_area;					// 区服
		i32_actordataid m_id				= nguid::none_actordataid();		// 数据id
		bool			m_manage_dbclient	= false;							// 是否有数据库依赖
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

	class actor_base
	{
	protected:
		struct impl_actor_base;
		ngl::impl<impl_actor_base> m_impl_actor_base;
	protected:
		actor_base() = delete;
		actor_base(const actor_base&) = delete;
		actor_base(const actorparmbase& aparm);
	public:
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
		template <
			EPROTOCOL_TYPE	PROTYPE		// 协议类型
			, pbdb::ENUM_DB DBTYPE		// 数据类型
			, typename		TDBTAB		// 数据表
			, typename		TACTOR		// 持有该数据表的actor
		>
		bool handle(message<np_actordb_load_response<PROTYPE, DBTYPE, TDBTAB>>& adata);
#pragma endregion 

#pragma region virtual_function
		virtual ~actor_base();

		//# 初始化数据
		//# 一般actor对象会在其重载虚函数中
		//# 让dbclient与actor对象进行绑定
		virtual void		init() {}

		//# 获取actor状态
		virtual actor_stat	get_activity_stat()						= 0;

		//# 设置actor状态
		virtual void		set_activity_stat(actor_stat astat)		= 0;

		//# 检查任务列表是否为空
		virtual bool		list_empty()							= 0;

		//# 进行任务
		virtual void		actor_handle(i32_threadid athreadid)	= 0;

		//# 添加任务
		virtual void		push(handle_pram& apram)				= 0;

		//# 清空当前任务
		virtual void		clear_task()							= 0;

		//# 执行handle之后调用
		virtual void		handle_after(handle_pram&) {}

		//# 派生actor重载此函数 会在数据加载完成后调用
		virtual void		loaddb_finish(bool adbishave) {}
#pragma endregion 

		//# 删除actor时候会被调用
		virtual void	release() = 0;

		//# 保存dbclient
		virtual void	save();

		//# 是否为单例
		bool			is_single();

		//# 获取actor guid
		nguid&			guid();

		//# 获取actor guid i64_actorid
		i64_actorid		id_guid();

		//# 获取actor guid的数据id
		i32_actordataid id();

		//# 获取actor guid的区服id
		i16_area		area();

		//# 获取actor guid的actor type
		ENUM_ACTOR		type();

		//# 移除actor自身
		void			erase_actor_byid();

		//# 移除指定actor
		static void		erase_actor_byid(const nguid& aguid);

		//# 向指定actor添加任务
		static void		push_task_id(const nguid& aguid, handle_pram& apram, bool abool);

		//# 给actor自身添加任务
		void			push_task_id(handle_pram& apram, bool abool);

		//# 给指定类型的actor添加任务
		void			push_task_type(ENUM_ACTOR atype, handle_pram& apram, bool aotherserver = false);

#pragma region network_strat
		//# 生成包
		template <typename T>
		static std::shared_ptr<pack> net_pack(T& adata, i64_actorid aactorid, i64_actorid arequestactorid);

		//# 发送数据到指定服务器
		template <typename T>
		static bool send_server(i32_serverid aserverid, T& adata, i64_actorid aactorid, i64_actorid arequestactorid);

		//# 向一组服务器发送数据
		template <typename T>
		static bool send_server(const std::vector<i32_serverid>& aserverid, T& adata, i64_actorid aactorid, i64_actorid arequestactorid);

		//# 发送pack到指定服务器
		template <typename T>
		static bool sendpacktoserver(i32_serverid aserverid, std::shared_ptr<pack>& apack);

		//# 给指定连接发送数据
		template <typename T>
		static bool sendpackbysession(i32_sessionid asession, std::shared_ptr<pack>& apack);

		//# 给指定连接发送数据
		template <typename T>
		static bool send(i32_sessionid asession, T& adata, i64_actorid aactorid, i64_actorid arequestactorid);

#pragma region network_kcp

		i32_session m_kcpsession = -1;

		//# 设置udp.kcp session
		void set_kcpssion(i32_session asession);

		//# 是否支持udp.kcp
		static bool iskcp();

		//# 通过udp.kcp发送数据
		template <typename T>
		bool sendkcp(T& adata, i64_actorid aactorid, int16_t asystemindex = 0);


		template <typename T>
		static bool static_sendkcp(
			i32_sessionid asession
			, T& adata
			, i64_actorid aactorid
			, i64_actorid arequestactorid
			, int16_t asystemindex = 0
		);

		template <typename T>
		static bool static_sendkcp(
			const std::vector<i32_sessionid>& asession
			, T& adata
			, i64_actorid aactorid
			, i64_actorid arequestactorid
			, int16_t asystemindex = 0
		);

#pragma region network_kcpclient
		virtual const char* kcpsessionmd5();

		bool connect_kcp(int16_t anum, const std::string& aip, i16_port aprot);
#pragma endregion //network_kcpclient

#pragma endregion //network_kcp
public:
		template <typename T>
		using tactor_forward = np_actor_forward<T, EPROTOCOL_TYPE_PROTOCOLBUFF, true, T>;
private:
		template <typename T>
		static void actor_forward_init(
			tactor_forward<T>& apro
			, i64_actorid aid
			)
		{
			nguid lguid(aid);
			apro.m_uid.push_back(lguid.actordataid());
			apro.m_area.push_back(lguid.area());
		}

		template <typename T>
		static void actor_forward_setdata(
			tactor_forward<T>& apro
			, std::shared_ptr<T>& adata
		)
		{
			apro.set_data(adata);
		}

		static i64_actorid actorclient_guid()
		{
			static i64_actorid lid = nguid::make(
				ACTOR_CLIENT
				, ttab_servers::tab()->m_area
				, nguid::none_actordataid()
			);
			return lid;
		}
	public:
		//# 根据actor_role.guidid给所在客户端发送数据
		template <typename T>
		static void send_client(i64_actorid aid, std::shared_ptr<T>& adata)
		{
			if (aid == nguid::make())
				return;
			auto pro = std::make_shared<tactor_forward<T>>();
			actor_forward_init(*pro, aid);
			actor_forward_setdata(*pro, adata);
			handle_pram lpram = handle_pram::create(aid, nguid::make(), pro);
			push_task_id(actorclient_guid(), lpram, true);
		}

		//# 向指定的gateway发送数据 actor_role.guidid用来确定是哪个客户端 
		template <typename T>
		static void send_client(i32_gatewayid agatewayid, i64_actorid aid, std::shared_ptr<T>& adata)
		{
			tab_servers* tab = ttab_servers::tab(agatewayid);
			if (tab == nullptr)
				return;
			if (tab->m_type != ngl::NODE_TYPE::GATEWAY)
				return;
			auto pro = std::make_shared<tactor_forward<T>>();
			actor_forward_init(*pro, aid);
			actor_forward_setdata(*pro, adata);
			send_server(agatewayid, *pro.get(), nguid::make(), aid);
		}

		template <typename T>
		static void send_client(const std::vector<i32_gatewayid>& agatewayid, i64_actorid aid, std::shared_ptr<T>& adata)
		{
			auto pro = std::make_shared<tactor_forward<T>>();
			actor_forward_init(*pro, aid);
			actor_forward_setdata(*pro, adata);
			send_server(agatewayid, *pro.get(), nguid::make(), aid);
		}
	private:
		template <typename T, typename ITOR>
		static void client_pro(ITOR abeg, ITOR aend, std::shared_ptr<T>& adata)
		{
			if (abeg == aend)
				return;
			auto pro = std::make_shared<tactor_forward<T>>();
			std::for_each(abeg, aend, [&pro](i64_actorid aactorid)
				{
					actor_forward_init(*pro, aactorid);
				});
			actor_forward_setdata(*pro, adata);
			handle_pram lpram = handle_pram::create(*abeg, nguid::make(), pro);
			//handle_pram lpram;
			//pram_create(pro, lpram, *abeg);
			push_task_id(actorclient_guid(), lpram, true);
		}
	public:
		//# 根据actor_role.guidid确定客户端，
		//# 给一组客户端发送数据
		template <typename T>
		static void send_client(std::initializer_list<i64_actorid>& alist, std::shared_ptr<T>& adata)
		{
			client_pro(alist.begin(), alist.end(), adata);
		}

		template <typename T>
		static void send_client(const std::vector<i64_actorid>& avecid, std::shared_ptr<T>& adata)
		{
			client_pro(avecid.begin(), avecid.end(), adata);
		}

		template <typename T>
		static void send_client(const std::list<i64_actorid>& avecid, std::shared_ptr<T>& adata)
		{
			client_pro(avecid.begin(), avecid.end(), adata);
		}

		template <typename T>
		static void send_client(const std::set<i64_actorid>& asetid, std::shared_ptr<T>& adata)
		{
			client_pro(asetid.begin(), asetid.end(), adata);
		}

	private:
		static std::vector<i32_serverid> m_gatewayids;
	public:
		static void init_gatewayser()
		{
			m_gatewayids.clear();
			if (ttab_servers::get_server(GATEWAY, ttab_servers::tab()->m_area, m_gatewayids) == false)
			{
				return;
			}
		}

		//# 向所有客户端发送消息
		template <typename T>
		static void send_allclient(std::shared_ptr<T>& adata)
		{
			if (m_gatewayids.empty())
			{
				return;
			}
			send_client(m_gatewayids, nguid::make(), adata);
		}

		//# 往指定区服所有客户端发送消息
		template <typename T>
		static void send_allclient(i16_area aarea, std::shared_ptr<T>& adata)
		{
			ttab_servers::foreach_server(GATEWAY, aarea, [&adata](const tab_servers* atab)
				{
					send_client(atab->m_id, nguid::make(), adata);
				});
		}

		//# 向指定actor发送数据
		template <typename T, bool IS_SEND = true>
		void send_actor(const nguid& aguid, std::shared_ptr<T>& adata)
		{
			handle_pram lpram = handle_pram::create<T, IS_SEND>(aguid, guid(), adata);
			push_task_id(aguid, lpram, true);
		}

		//# 向指定actor发送数据
		template <typename T, bool IS_SEND = true>
		void send_actor(const nguid& aguid, std::shared_ptr<T>& adata, const std::function<void()>& afailfun)
		{
			handle_pram lpram = handle_pram::create<T, IS_SEND>(aguid, guid(), adata, afailfun);
			push_task_id(aguid, lpram, true);
		}

		//# 向指定actor发送pack
		void send_actor_pack(const nguid& aguid, std::shared_ptr<pack>& adata)
		{
			handle_pram lpram = handle_pram::create_pack(aguid, guid(), adata);
			push_task_id(aguid, lpram, true);
		}

		//# 群发给指定类型的所有actor
		template <typename T, bool IS_SEND = true>
		void send_actor(ENUM_ACTOR atype, std::shared_ptr<T>& adata, bool aotherserver = false)
		{
			handle_pram lpram = handle_pram::create<T, IS_SEND>(nguid::make_self(atype), guid(), adata);
			push_task_type(atype, lpram, aotherserver);
		}

		//# 发送数据到指定的actor
		template <typename T, bool IS_SEND = true>
		static void static_send_actor(const nguid& aguid, const nguid& arequestguid, std::shared_ptr<T>& adata)
		{
			handle_pram lpram = handle_pram::create<T, IS_SEND>(aguid, arequestguid, adata);
			push_task_id(aguid, lpram, true);
		}

		//# 发送数据到指定的actor
		template <typename T, bool IS_SEND = true>
		static void static_send_actor(const nguid& aguid, const nguid& arequestguid, std::shared_ptr<T>& adata, const std::function<void()>& afailfun)
		{
			handle_pram lpram;
			handle_pram::create<T, IS_SEND>(lpram, aguid, arequestguid, adata, afailfun);
			push_task_id(aguid, lpram, true);
		}
#pragma region network_strat_group
	private:
		struct group_info
		{
			ENUM_ACTOR				m_actortype;
			std::set<i64_actorid>	m_actorlist;
		};
		//# 分组数据
		std::map<int, group_info>	m_group;
		int							m_currentoffset = 0;
	public:
		//# 创建一个群发分组(可以指定ActorType,主要是为了区分客户端与普通actor)
		int add_group(ENUM_ACTOR aactortype = ACTOR_NONE);
		
		//# 移除一个分组
		void remove_group(int agroupid);
		
		//# 将成员加入某个群发分组
		bool add_group_member(int agroupid, i64_actorid amember);
		
		//# 将成员从某个群发分组中移除
		void remove_group_member(int agroupid, i64_actorid amember);
		
		//# 给一组成员发送消息
		template <typename T>
		void send_group(int agroupid, std::shared_ptr<T>& adata)
		{
			auto itor = m_group.find(agroupid);
			if (itor == m_group.end())
				return;
			if (itor->second.m_actortype != ACTOR_ROBOT)
			{
				handle_pram lpram;
				handle_pram::create<T>(lpram, nguid::make(), guid(), adata);
				for (i64_actorid actorid : itor->second)
				{
					lpram.m_actor = actorid;
					push_task_id(actorid, lpram, true);
				}
			}
			else
			{
				std::initializer_list<i64_actorid> ltemp(itor->second.m_actorlist);
				send_client(ltemp, adata);
			}
			return;
		}
#pragma endregion
		// 发送数据到指定的actor
		template <typename T>
		static void static_send_actor(const nguid& aguid, const nguid& arequestguid, std::shared_ptr<T>&& adata)
		{
			handle_pram lpram;
			handle_pram::create<T>(lpram, aguid, arequestguid, adata);
			push_task_id(aguid, lpram, true);
		}
#pragma endregion
		
#pragma region timer
	private:
		//# 间隔一段时间发起的全员(所有actor)广播
		//# 可以在这个广播里推送一些需要处理的任务,例如 保存数据
		static int	m_broadcast;			// 推送全员广播的 单位(毫秒)
		static int	m_broadcasttimer;		// 推送广播的定时器id
		bool		m_isbroadcast;			// 是否接收广播消息
	public:
		//# 设置定时任务参数
		int32_t		set_timer(const timerparm& aparm);

		//# 是否支持广播
		bool		isbroadcast();

		//# 设置是否支持广播
		void		set_broadcast(bool aisbroadcast);

		//# 启动广播定时器
		static void start_broadcast();
#pragma endregion 

		//# 日志相关
		static std::shared_ptr<np_actor_logitem> m_nonelog;
		std::shared_ptr<np_actor_logitem> log_debug(const std::source_location& asource = std::source_location::current());
		std::shared_ptr<np_actor_logitem> log_debug_net(const std::source_location& asource = std::source_location::current());
		std::shared_ptr<np_actor_logitem> log_info(const std::source_location& asource = std::source_location::current());
		std::shared_ptr<np_actor_logitem> log_info_net(const std::source_location& asource = std::source_location::current());
		std::shared_ptr<np_actor_logitem> log_warn(const std::source_location& asource = std::source_location::current());
		std::shared_ptr<np_actor_logitem> log_warn_net(const std::source_location& asource = std::source_location::current());
		std::shared_ptr<np_actor_logitem> log_error(const std::source_location& asource = std::source_location::current());
		std::shared_ptr<np_actor_logitem> log_error_net(const std::source_location& asource = std::source_location::current());

		//# actor_base::create 
		//# 构造actor对象会自动被调用
		template <typename TDerived>
		static void first_nregister(ENUM_ACTOR atype)
		{
			static bool lfirst = true;
			if (lfirst == false)
				return;
			lfirst = false;
			TDerived::nregister();	
			if (atype != ngl::ACTOR_LOG)
			{
				nlogactor lnlogactor(atype, ngl::ELOG_LOCAL);
				ngl::actor_base::create(ngl::ACTOR_LOG, lnlogactor.m_value32);
			}
		}

		//# 用于创建非单例actor
		static actor_base* create(ENUM_ACTOR atype, i32_actordataid aid, void* aparm = nullptr);
	};

	template <typename T>
	class actor_instance
	{
	public:
		// 定义在 actor_manage.h
		static T& instance();
	};
}//namespace ngl
