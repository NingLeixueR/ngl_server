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

	class actor_base
	{
	protected:
		struct impl_actor_base;
		ngl::impl<impl_actor_base> m_impl_actor_base;
	protected:
		actor_base(const actorparmbase& aparm);
	public:
#pragma region db
		std::unique_ptr<actor_manage_dbclient>& get_actor_manage_dbclient();
		bool			isload();
		void			set_db_component(db_component* acomponent);
		void			db_component_init_data();
		void			init_db_component(bool acreate);
		bool			isloadfinish();
		void			add_dbclient(actor_dbclient_base* adbclient, i64_actorid aid);
		template <EPROTOCOL_TYPE PROTYPE, ENUM_DB DBTYPE, typename TDBTAB, typename TACTOR>
		bool			handle(i32_threadid athread, const std::shared_ptr<pack>& apack, actor_db_load_response<PROTYPE, DBTYPE, TDBTAB>& adata);
#pragma endregion 
#pragma region virtual_function
		virtual ~actor_base();
		virtual void init() {}
		virtual actor_stat get_activity_stat() = 0;
		virtual void set_activity_stat(actor_stat astat) = 0;
		virtual bool list_empty() = 0;
		virtual void actor_handle(i32_threadid athreadid) = 0;
		virtual void push(handle_pram& apram) = 0;
		virtual void loaddb_finish(bool adbishave) {} // 派生actor重载此函数 会在数据加载完成后调用
		virtual void clear_task() = 0;// 清空当前任务
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

		//sendtoserver
		template <typename T>
		static bool sendtoserver(i32_serverid aserverid, T& adata, i64_actorid aactorid, i64_actorid arequestactorid);

		template <typename T>
		static bool sendpacktoserver(i32_serverid aserverid, std::shared_ptr<pack>& apack);

		template <typename T>
		static bool sendpackbysession(i32_sessionid asession, std::shared_ptr<pack>& apack);

		template <typename T>
		static bool send(i32_sessionid asession, T& adata, i64_actorid aactorid, i64_actorid arequestactorid);

		template <typename T>
		void send_client(std::shared_ptr<T>& adata)
		{
			send_client(id_guid(), adata);
		}

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
			handle_pram::create<actor_forward<T, EPROTOCOL_TYPE_PROTOCOLBUFF, true, T>,true, true, true>(
				lpram, 
				lguid,
				actor_guid::make(), 
				pro
			);
			push_task_id(lclientguid, lpram, true);
		}

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
				sendtoserver(agatewayid, pro, actor_guid::make(), aid);
			}
		}

		template <typename T>
		static void send_client(const std::vector<i64_actorid>& avecid, std::shared_ptr<T>& adata)
		{
			if(avecid.empty())
				return;
			handle_pram lpram;
			auto pro = std::make_shared<actor_forward<T, EPROTOCOL_TYPE_PROTOCOLBUFF, true, T>>();
			for (i64_actorid id : avecid)
			{
				actor_guid lguid(id);
				pro->m_uid.push_back(lguid.actordataid());
				pro->m_area.push_back(lguid.area());
			}
			pro->set_data(adata);
			actor_guid lclientguid = actor_guid::make(ACTOR_ADDRESS_CLIENT, ttab_servers::tab()->m_area, actor_guid::none_actordataid());
			handle_pram::create<actor_forward<T, EPROTOCOL_TYPE_PROTOCOLBUFF, true, T>, true, true, true>(
				lpram,
				avecid[0],
				actor_guid::make(),
				pro
			);
			push_task_id(lclientguid, lpram, true);
		}

		// 往所有客户端发送消息
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

		// 发送给指定actor
		template <typename T>
		void send_actor(const actor_guid& aguid, std::shared_ptr<T>& adata)
		{
			handle_pram lpram; 
			handle_pram::create<T>(lpram, aguid, guid(), adata);
			push_task_id(aguid, lpram, true);
		}

		void send_actor_pack(const actor_guid& aguid, std::shared_ptr<pack>& adata)
		{
			handle_pram lpram;
			handle_pram::create_pack(lpram, aguid, guid(), adata);
			push_task_id(aguid, lpram, true);
		}


		template <typename T>
		void send_actor(ENUM_ACTOR atype, std::shared_ptr<T>& adata, bool aotherserver = false)
		{
			handle_pram lpram;
			handle_pram::create<T>(lpram, actor_guid::make_self(atype), guid(), adata);
			push_task_type(atype, lpram, aotherserver);
		}

		template <typename T>
		static void static_send_actor(const actor_guid& aguid, const actor_guid& arequestguid, std::shared_ptr<T>& adata)
		{
			handle_pram lpram;
			handle_pram::create<T>(lpram, aguid, arequestguid, adata);
			push_task_id(aguid, lpram, true);
		}

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
