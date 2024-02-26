#pragma once

#include <list>
#include <vector>
#include <set>
#include <map>
#include <atomic>
#include <list>

#include "actor_enum.h"
#include "actor_guid.h"
#include "localtime.h"
#include "handle_pram.h"
#include "actor_timer.h"
#include "type.h"
#include "nlog.h"
#include "impl.h"
#include "ttab_servers.h"
#include "udp_kcp.h"

namespace ngl
{
	class actor_base;
	class actor_dbclient_base;
	class actor_manage_dbclient;
	class db_component;

	struct actorparmbase
	{
		ENUM_ACTOR m_type		= actor_guid::none_type();				// actor类型
		i16_area m_area			= tab_self_area;						// 区服
		i32_actordataid m_id	= actor_guid::none_actordataid();		// 数据id
		bool m_manage_dbclient	= false;								// 是否有数据库依赖
	};

	enum actor_stat
	{
		actor_stat_init,  // 初始化阶段
		actor_stat_free,  // 状态为空闲
		actor_stat_list,  // 繁忙-在等待线程的actor队列中
		actor_stat_run,	  // 繁忙-在线程中执行任务
		actor_stat_close, // 关闭状态
	};

	template <typename T>
	struct message;

	class actor_base
	{
	protected:
		struct impl_actor_base;
		ngl::impl<impl_actor_base> m_impl_actor_base;
	protected:
		actor_base(const actorparmbase& aparm);
	public:
#pragma region db
		// 获取actor_manage_dbclient实例
		std::unique_ptr<actor_manage_dbclient>& get_actor_manage_dbclient();
		// 是否需要从数据库加载数据
		bool			isload();
		// 是否加载完成
		bool			isloadfinish();
		// 设置db_component组件
		void			set_db_component(db_component* acomponent);
		// 初始化数据(在数据加载完成后)
		void			db_component_init_data();
		void			init_db_component(bool acreate);
		void			add_dbclient(actor_dbclient_base* adbclient, i64_actorid aid);
		template <EPROTOCOL_TYPE PROTYPE, pbdb::ENUM_DB DBTYPE, typename TDBTAB, typename TACTOR>
		bool			handle(message<actor_db_load_response<PROTYPE, DBTYPE, TDBTAB>>& adata);
#pragma endregion 
#pragma region virtual_function
		virtual ~actor_base();
		virtual void init() {}
		virtual actor_stat get_activity_stat() = 0;
		virtual void set_activity_stat(actor_stat astat) = 0;
		virtual bool list_empty() = 0;
		virtual void actor_handle(i32_threadid athreadid) = 0;
		virtual void push(handle_pram& apram) = 0;
		// 派生actor重载此函数 会在数据加载完成后调用
		virtual void loaddb_finish(bool adbishave) {} 
		// 清空当前任务
		virtual void clear_task() = 0;
		// 执行handle之后调用
		virtual void handle_after() {}
#pragma endregion 

		// 删除actor时候会被调用
		virtual void	release() = 0;
		virtual void	save();
		bool			is_single();
		actor_guid&		guid();
		i64_actorid		id_guid();
		i32_actordataid id();
		i16_area		area();
		ENUM_ACTOR		type();
		void			erase_actor_byid();
		static void		erase_actor_byid(const actor_guid& aguid);
		static void		push_task_id(const actor_guid& aguid, handle_pram& apram, bool abool);
		void			push_task_id(handle_pram& apram, bool abool);
		void			push_task_type(ENUM_ACTOR atype, handle_pram& apram, bool aotherserver = false);

#pragma region network_strat
		// 生成包
		template <typename T>
		static std::shared_ptr<pack> net_pack(T& adata, i64_actorid aactorid, i64_actorid arequestactorid);

		// 发送数据到指定服务器
		template <typename T>
		static bool send_server(i32_serverid aserverid, T& adata, i64_actorid aactorid, i64_actorid arequestactorid);

		// 发送pack到指定服务器
		template <typename T>
		static bool sendpacktoserver(i32_serverid aserverid, std::shared_ptr<pack>& apack);

		// 给指定连接发送数据
		template <typename T>
		static bool sendpackbysession(i32_sessionid asession, std::shared_ptr<pack>& apack);

		// 给指定连接发送数据
		template <typename T>
		static bool send(i32_sessionid asession, T& adata, i64_actorid aactorid, i64_actorid arequestactorid);

		template <typename T>
		static bool sendbykcp(i32_sessionid asession, T& adata, i64_actorid aactorid, i64_actorid arequestactorid)
		{
			udp_kcp::getInstance().send(asession, adata, aactorid, arequestactorid);
			return true;
		}

		i32_session m_kcpsession = -1;

		void set_kcpssion(i32_session asession)
		{
			if (m_kcpsession == -1)
				m_kcpsession = asession;
		}

		template <typename T>
		bool sendkcp(T& adata, i64_actorid aactorid)
		{
			if (m_kcpsession == -1)
				return false;
			tab_servers* tab = ttab_servers::tab();
			if (tab->m_isopenkcp == false)
				return false;
			udp_kcp::getInstance().send(m_kcpsession, adata, aactorid, id_guid());
			return true;
		}

		template <typename T>
		static bool static_sendkcp(i32_sessionid asession, T& adata, i64_actorid aactorid, i64_actorid arequestactorid)
		{
			tab_servers* tab = ttab_servers::tab();
			if (tab->m_isopenkcp == false)
				return false;
			udp_kcp::getInstance().send(asession, adata, aactorid, arequestactorid);
			return true;
		}

		template <typename T>
		static bool static_sendkcp(
			const std::vector<i32_sessionid>& asession
			, T& adata
			, i64_actorid aactorid
			, i64_actorid arequestactorid
		)
		{
			tab_servers* tab = ttab_servers::tab();
			if (tab->m_isopenkcp == false)
				return false;
			udp_kcp::getInstance().send(asession, adata, aactorid, arequestactorid);
			return true;
		}

		virtual const char* kcpsessionmd5()
		{
			return "";
		}

		bool connect_kcp(const std::string& aip, i16_port aprot)
		{
			tab_servers* tab = ttab_servers::tab();
			if (tab->m_isopenkcp == false)
				return false;
			
			std::string lkcpsessionmd5 = kcpsessionmd5();
			if (lkcpsessionmd5 == "")
				return false;
			udp_kcp::getInstance().connect(lkcpsessionmd5, id_guid(), aip, aprot, [this](i32_session asession)
				{
					m_kcpsession = asession;
					std::cout << "m_kcpsession = " << m_kcpsession << std::endl;
				});
			return true;
		}

		// 根据actor_role.guidid给所在客户端发送数据
		template <typename T>
		static void send_client(i64_actorid aid, std::shared_ptr<T>& adata)
		{
			handle_pram lpram;
			auto pro = std::make_shared<actor_forward<T, EPROTOCOL_TYPE_PROTOCOLBUFF, true, T>>();
			actor_guid lguid(aid);
			pro->m_uid.push_back(lguid.actordataid());
			pro->m_area.push_back(lguid.area());
			pro->set_data(adata);
			actor_guid lclientguid = actor_guid::make(ACTOR_ADDRESS_CLIENT, ttab_servers::tab()->m_area, actor_guid::none_actordataid());
			using tactor_forward = actor_forward<T, EPROTOCOL_TYPE_PROTOCOLBUFF, true, T>;
			handle_pram::create<tactor_forward,true, true>(lpram, lguid, actor_guid::make(), pro);
			push_task_id(lclientguid, lpram, true);
		}

		// 向指定的gateway发送数据 actor_role.guidid用来确定是哪个客户端 
		template <typename T>
		static void send_client(i32_gatewayid agatewayid, i64_actorid aid, std::shared_ptr<T>& adata)
		{
			if (agatewayid != 0)
			{
				actor_forward<T, EPROTOCOL_TYPE_PROTOCOLBUFF, true, T> pro;
				actor_guid lguid(aid);
				pro.m_uid.push_back(lguid.actordataid());
				pro.m_area.push_back(lguid.area());
				pro.set_data(adata);
				send_server(agatewayid, pro, actor_guid::make(), aid);
			}
		}
	private:
		template < typename T, typename ITOR>
		static void client_pro(ITOR abeg, ITOR aend, std::shared_ptr<T>& adata)
		{
			if (abeg == aend)
				return;
			handle_pram lpram;
			auto pro = std::make_shared<actor_forward<T, EPROTOCOL_TYPE_PROTOCOLBUFF, true, T>>();
			std::for_each(abeg, abeg, [&pro](i64_actorid aactorid)
				{
					actor_guid lguid(aactorid);
					pro->m_uid.push_back(lguid.actordataid());
					pro->m_area.push_back(lguid.area());
				});
			pro->set_data(adata);
			actor_guid lclientguid = actor_guid::make(ACTOR_ADDRESS_CLIENT, ttab_servers::tab()->m_area, actor_guid::none_actordataid());
			actor_guid lguid(*abeg);
			handle_pram::create(lpram, lguid, actor_guid::make(), pro);
			push_task_id(lclientguid, lpram, true);
		}
	public:
		// 根据actor_role.guidid确定客户端，给一组客户端发送数据
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

		// 向所有客户端发送消息
		template <typename T>
		static void send_allclient(std::shared_ptr<T>& adata)
		{
			ttab_servers::foreach_server(GATEWAY, [&adata](const tab_servers* atab)
				{
					send_client(atab->m_id, actor_guid::make(), adata);
				});
		}

		// 往指定区服所有客户端发送消息
		template <typename T>
		static void send_allclient(i16_area aarea, std::shared_ptr<T>& adata)
		{
			ttab_servers::foreach_server(GATEWAY, aarea, [&adata](const tab_servers* atab)
				{
					send_client(atab->m_id, actor_guid::make(), adata);
				});
		}

		// 向指定actor发送数据
		template <typename T, bool IS_SEND = true>
		void send_actor(const actor_guid& aguid, std::shared_ptr<T>& adata)
		{
			handle_pram lpram; 
			handle_pram::create<T, IS_SEND>(lpram, aguid, guid(), adata);
			push_task_id(aguid, lpram, true);
		}

		template <typename T, bool IS_SEND = true>
		void send_actor(const actor_guid& aguid, std::shared_ptr<T>& adata, const std::function<void()>& afailfun)
		{
			handle_pram lpram;
			handle_pram::create<T, IS_SEND>(lpram, aguid, guid(), adata, afailfun);
			push_task_id(aguid, lpram, true);
		}

		// 向指定actor发送pack
		void send_actor_pack(const actor_guid& aguid, std::shared_ptr<pack>& adata)
		{
			handle_pram lpram;
			handle_pram::create_pack(lpram, aguid, guid(), adata);
			push_task_id(aguid, lpram, true);
		}

		// 群发给指定类型的所有actor
		template <typename T, bool IS_SEND = true>
		void send_actor(ENUM_ACTOR atype, std::shared_ptr<T>& adata, bool aotherserver = false)
		{
			handle_pram lpram;
			handle_pram::create<T, IS_SEND>(lpram, actor_guid::make_self(atype), guid(), adata);
			push_task_type(atype, lpram, aotherserver);
		}

		// 发送数据到指定的actor
		template <typename T, bool IS_SEND = true>
		static void static_send_actor(const actor_guid& aguid, const actor_guid& arequestguid, std::shared_ptr<T>& adata)
		{
			handle_pram lpram;
			handle_pram::create<T, IS_SEND>(lpram, aguid, arequestguid, adata);
			push_task_id(aguid, lpram, true);
		}

		template <typename T, bool IS_SEND = true>
		static void static_send_actor(const actor_guid& aguid, const actor_guid& arequestguid, std::shared_ptr<T>& adata, const std::function<void()>& afailfun)
		{
			handle_pram lpram;
			handle_pram::create<T, IS_SEND>(lpram, aguid, arequestguid, adata, afailfun);
			push_task_id(aguid, lpram, true);
		}

#pragma region network_strat_group
	private:
		struct group_info
		{
			ENUM_ACTOR m_actortype;
			std::set<i64_actorid> m_actorlist;
		};
		// 分组数据
		std::map<int, group_info> m_group;
		int m_currentoffset = 0;
	public:
		// 创建一个群发分组(可以指定ActorType,主要是为了区分客户端与普通actor)
		int add_group(ENUM_ACTOR aactortype = ACTOR_NONE);
		// 移除一个分组
		void remove_group(int agroupid);
		// 将成员加入某个群发分组
		bool add_group_member(int agroupid, i64_actorid amember);
		// 将成员从某个群发分组中移除
		void remove_group_member(int agroupid, i64_actorid amember);
		// 给一组成员发送消息
		template <typename T>
		void send_group(int agroupid, std::shared_ptr<T>& adata)
		{
			auto itor = m_group.find(agroupid);
			if (itor == m_group.end())
				return;
			if (itor->second.m_actortype != ACTOR_ROBOT)
			{
				handle_pram lpram;
				handle_pram::create<T>(lpram, actor_guid::make(), guid(), adata);

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
		static void static_send_actor(const actor_guid& aguid, const actor_guid& arequestguid, std::shared_ptr<T>&& adata)
		{
			handle_pram lpram;
			handle_pram::create<T>(lpram, aguid, arequestguid, adata);
			push_task_id(aguid, lpram, true);
		}

#pragma endregion
		
#pragma region timer
	private:
		// ## 间隔一段时间发起的全员(所有actor)广播
		// ## 可以在这个广播里推送一些需要处理的任务,例如 保存数据
		static int m_broadcast;			// 推送全员广播的 单位(毫秒)
		static int m_broadcasttimer;	// 推送广播的定时器id
		// ## 是否接收广播消息
		bool m_isbroadcast;
	public:
		int32_t		set_timer(const timerparm& aparm);
		bool		isbroadcast();
		void		set_broadcast(bool aisbroadcast);
		static void start_broadcast();
#pragma endregion 
		template <typename TDerived>
		static void first_actor_register()
		{
			static bool lfirst = true;
			if (lfirst)
			{
				lfirst = false;
				TDerived::actor_register();
			}
		}

		// 用于创建非单例actor
		static actor_base* create(ENUM_ACTOR atype, i32_actordataid aid, void* aparm = nullptr);
	};

	template <typename T>
	class actor_instance
	{
	public:
		// 定义在 actor_manage.h
		static T& instance();
	};

}
