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

#include "actor/actor_base/actor_client.h"
#include "actor/actor_base/actor_base.h"
#include "actor/actor_base/naddress.h"
#include "actor/actor_base/nthread.h"
#include "tools/serialize/ndefine.h"
#include "tools/serialize/pack.h"
#include "tools/threadtools.h"

#include <list>
#include <map>
#include <set>

namespace ngl
{
	class actor_manage
	{
	private:
		actor_manage(const actor_manage&) = delete;
		actor_manage& operator=(const actor_manage&) = delete;

		using ptrnthread = std::shared_ptr<nthread>;
		std::list<ptrnthread>		m_workthreads;		// 工作线程
		std::list<ptrnthread>		m_workthreadscopy;	// 工作线程(只在初始化是拷贝一份，保证使用时不会析构)
		bool						m_suspend = false;	// 是否挂起
		std::list<ptrnthread>		m_suspendthreads;	// 挂起的工作线程
		std::jthread				m_thread;			// 管理线程
		i32_threadsize				m_threadnum = -1;	// 工作线程数量
		std::map<nguid, ptractor>	m_actorbyid;		// 索引actor
		std::map<nguid, ptractor>	m_actorbroadcast;	// 支持广播的actor
		std::list<ptractor>			m_actorlist;		// 有任务的actor列表
		std::set<i16_actortype>		m_actortype;		// 包含哪些actortype
		std::map<nguid, std::function<void()>>			m_delactorfun;	// 删除actor后需要执行的操作// (延迟操作:删除的瞬间actor正是运行状态,等待其回归后进行删除)
		std::map<ENUM_ACTOR, std::map<nguid, ptractor>> m_actorbytype;	// 按类型索引actor

		ngl_lockinit;

		actor_manage();
		~actor_manage() = default;

		// # nosafe_开头的函数代表"内部操作未加锁"，不允许类外调用
		// # 根据guid获取actor实例
		ptractor& nosafe_get_actor(const nguid& aguid);

		// # 根据guid获取actor实例,如果本结点没有找到该actor实例，则根据结点类型获取(actor_client/actor_server)的guid，用于转发
		ptractor& nosafe_get_actorbyid(const nguid& aguid, handle_pram& apram);

		// # 向actor实例插入任务
		void nosafe_push_task_id(const ptractor& lpactor, handle_pram& apram);

		// # actor_manage 调度actor实例处理任务的线程实例
		void run();
	public:
		static actor_manage& instance()
		{
			static actor_manage ltemp;
			return ltemp;
		}

		//# 获取
		nguid get_clientguid();

		//# 初始化 设置工作线程数量
		void init(i32_threadsize apthreadnum);

		//# 获取本进程存在的actor类型
		void get_type(std::vector<i16_actortype>& aactortype);

		//# 根据node类型获取(actor_client/actor_server)的guid
		nguid nodetypebyguid();

		// # 将消息T封装后传递给指定guid的actor
		template <typename T, bool IS_SEND = true>
		inline void push_task_id(const nguid& aguid, std::shared_ptr<T>& apram)
		{
			handle_pram lparm = handle_pram::create<T, IS_SEND>(aguid, nguid::make(), apram);
			push_task_id(aguid, lparm);
		}

		// # 添加actor
		bool add_actor(actor_base* apactor, const std::function<void()>& afun);

		// # 添加actor
		bool add_actor(const ptractor& apactor, const std::function<void()>& afun);

		// # 移除actor
		void erase_actor(const nguid& aguid, const std::function<void()>& afun = nullptr);

		// # 是否存在某个actor
		bool is_have_actor(const nguid& aguid);

		// # 工作线程将actor添加到m_actorlist
		void push(const ptractor& apactor, ptrnthread atorthread = nullptr);

		// # 向actor中添加任务
		void push_task_id(const nguid& aguid, handle_pram& apram);
		void push_task_id(const std::set<i64_actorid>& asetguid, handle_pram& apram);

		// # 向某个类型的actor中添加任务
		void push_task_type(ENUM_ACTOR atype, handle_pram& apram);

		// # 向当前进程所有actor广播消息
		void broadcast_task(handle_pram& apram);

		// # 暂时挂起所有线程，已执行单步操作(热更数据表)
		void statrt_suspend_thread();
		void finish_suspend_thread();

		// # 获取actor数量
		int32_t actor_count();

		// # 获取actor stat 数据
		void get_actor_stat(msg_actor_stat& adata);
	};

	// # 暂时挂起 actor_manage
	// # 自动调用
	// # 构造调用actor_manage.statrt_suspend_thread
	// # 析构调用actor_manage.finish_suspend_thread
	class actor_suspend
	{
		actor_suspend(const actor_suspend&) = delete;
		actor_suspend& operator=(const actor_suspend&) = delete;
	public:
		actor_suspend();
		~actor_suspend();
	};

	// # 进程单利actor 自动注册协议与自动添加actor_manage
	template <typename T>
	T& actor_instance<T>::instance()
	{
		static T ltemp;
		static std::atomic lfirst = true;
		if (lfirst.exchange(false))
		{
			T* lptemp = &ltemp;
			actor_manage::instance().add_actor(&ltemp, [lptemp]()
				{
					lptemp->set_activity_stat(actor_stat_free);
					lptemp->init();
					lptemp->init_db_component(false);
				}
			);
			ltemp.template init_rfun<T>();
			T::nregister();
		}
		return ltemp;
	}
}//namespace ngl

namespace ngl
{
	template <typename T, bool IS_SEND/* = true*/>
	void actor_base::send_actor(ENUM_ACTOR atype, const std::shared_ptr<T>& adata)
	{
		handle_pram lpram = handle_pram::create<T, IS_SEND>(nguid::make_self(atype), nguid::make(), adata);
		lpram.m_forwardtype = true;
		actor_manage::instance().push_task_type(atype, lpram);
	}

	template <typename T, bool IS_SEND/* = true*/>
	void actor_base::send_actor(const nguid& aguid, const nguid& arequestguid, const std::shared_ptr<T>& adata)
	{
		handle_pram lpram = handle_pram::create<T, IS_SEND>(aguid, arequestguid, adata);
		actor_manage::instance().push_task_id(aguid, lpram);
	}

	template <typename T, bool IS_SEND/* = true*/>
	void actor_base::send_actor(const nguid& aguid, const nguid& arequestguid, const std::shared_ptr<T>& adata, const std::function<void()>& afailfun)
	{
		handle_pram lpram = handle_pram::create<T, IS_SEND>(aguid, arequestguid, adata, afailfun);
		actor_manage::instance().push_task_id(aguid, lpram);
	}

	template <typename T, bool IS_SEND/* = true*/>
	void actor_base::send_actor(const std::set<i64_actorid>& asetguid, const nguid& arequestguid, const std::shared_ptr<T>& adata)
	{
		if (!asetguid.empty())
		{
			handle_pram lpram = handle_pram::create<T, IS_SEND>(asetguid, arequestguid, adata);
			actor_manage::instance().push_task_id(asetguid, lpram);
		}
	}

	template <typename T>
	void actor_base::send_client(const std::set<i64_actorid>& aids, const T& adata, ENET_PROTOCOL aprotocol/* = ENET_TCP*/)
	{
		auto pro = std::make_shared<np_actor_forward<T, forward_g2c<T>>>();
		pro->m_data.m_protocol = aprotocol;
		pro->m_data.m_data = adata;
		for (i64_actorid aactorid : aids)
		{
			nguid lguid(aactorid);
			pro->m_data.m_uid.push_back(lguid.actordataid());
			pro->m_data.m_area.push_back(lguid.area());
		}
		handle_pram lpram = handle_pram::create<T>(nguid::make(), nguid::make(), pro);
		actor_manage::instance().push_task_id(actorclient_guid(), lpram);
	}

	template <typename T>
	void actor_base::send_client(i64_actorid aid, const T& adata, ENET_PROTOCOL aprotocol/* = ENET_TCP*/)
	{
		std::set<i64_actorid> lids = { aid };
		send_client(lids, adata, aprotocol);
	}

	template <typename T>
	void actor_base::send_client(const std::vector<i64_actorid>& aids, const T& adata, ENET_PROTOCOL aprotocol/* = ENET_TCP*/)
	{
		std::set<i64_actorid> lids(aids.cbegin(), aids.cend());
		send_client(lids, adata, aprotocol);
	}

	template <typename T>
	void actor_base::send_client(const T& adata, ENET_PROTOCOL aprotocol/* = ENET_TCP*/)
	{
		std::set<i32_serverid>& lgatewayids = sysconfig::gatewayids();
		if (!lgatewayids.empty())
		{
			auto pro = std::make_shared<np_actor_forward<T, forward_g2c<T>>>();
			pro->m_data.m_protocol = aprotocol;
			pro->m_data.m_data = adata;
			nguid lguid(nguid::make());
			pro->m_data.m_uid.push_back(lguid.actordataid());
			pro->m_data.m_area.push_back(lguid.area());
			send_server(lgatewayids, *pro, nguid::make(), nguid::make());
		}
	}

	template <typename T>
	void actor_base::send_clientbyarea(i16_area aarea, const T& adata, ENET_PROTOCOL aprotocol/* = ENET_TCP*/)
	{
		auto pro = std::make_shared<np_actor_forward<T, forward_g2c<T>>>();
		pro->m_data.m_protocol = aprotocol;
		pro->m_data.m_data = adata;
		ttab_servers::instance().foreach_server(GATEWAY, aarea, [&pro](const tab_servers* atab)
			{
				send_server(atab->m_id, *pro, nguid::make(), nguid::make());
			}
		);
	}
}//namespace ngl