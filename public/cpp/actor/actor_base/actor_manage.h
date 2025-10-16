#pragma once

#include "actor_client.h"
#include "threadtools.h"
#include "actor_base.h"
#include "naddress.h"
#include "nthread.h"
#include "ndefine.h"
#include "pack.h"

#include <list>
#include <map>
#include <set>

namespace ngl
{
	class actor_manage
	{
		actor_manage(const actor_manage&) = delete;
		actor_manage& operator=(const actor_manage&) = delete;

		//// ---- 线程相关
		std::list<nthread*>	m_workthread;		// 工作线程
		bool				m_suspend = false;	// 是否挂起
		std::list<nthread*>	m_suspendthread;	// 挂起的工作线程
		std::jthread		m_thread;			// 管理线程
		i32_threadsize		m_threadnum = -1;	// 工作线程数量

		ngl_lockinit;

		// # 索引actor
		std::map<nguid, ptractor>							m_actorbyid;
		// # 支持广播的actor
		std::map<nguid, ptractor>							m_actorbroadcast;
		// # 按类型索引actor
		std::map<ENUM_ACTOR, std::map<nguid, ptractor>>		m_actorbytype;
		// # 有任务的actor列表
		std::list<ptractor>									m_actorlist;
		// # 包含哪些actortype
		std::set<i16_actortype>								m_actortype;
		// # 删除actor后需要执行的操作(延迟操作:删除的瞬间actor正是运行状态,等待其回归后进行删除)
		std::map<nguid, std::function<void()>>				m_delactorfun;
		// # actor就绪状态(如果需要加载db，db加载完成)
		std::set<nguid>										m_ready;

		actor_manage();
		~actor_manage();
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

		//# actor就绪状态(如果需要加载db，db加载完成)
		bool ready(const nguid& aguid);

		void set_ready(const nguid& aguid);

		nguid nodetypebyguid();

		ptractor& nosafe_get_actor(const nguid& aguid);

		ptractor& nosafe_get_actorbyid(const nguid& aguid, handle_pram& apram);

		// # nosafe_开头的函数代表"内部操作未加锁"，不允许类外调用
		void nosafe_push_task_id(const ptractor& lpactor, handle_pram& apram);

		template <typename T, bool IS_SEND = true>
		inline void push_task_id(const nguid& aguid, std::shared_ptr<T>& apram)
		{
			handle_pram lparm = handle_pram::create<T, IS_SEND>(aguid, nguid::make(), apram);
			push_task_id(aguid, lparm);
		}

		//# 添加actor
		bool add_actor(actor_base* apactor, const std::function<void()>& afun);

		//# 添加actor
		bool add_actor(const ptractor& apactor, const std::function<void()>& afun);

		//# 移除actor
		void erase_actor(const nguid& aguid, const std::function<void()>& afun = nullptr);

		//# 是否存在某个actor
		bool is_have_actor(const nguid& aguid);

		//# 工作线程将actor添加到m_actorlist
		void push(const ptractor& apactor, nthread* atorthread = nullptr);

		//# 向actor中添加任务
		void push_task_id(const nguid& aguid, handle_pram& apram);
		void push_task_id(const std::set<i64_actorid>& asetguid, handle_pram& apram);

		//# 向某个类型的actor中添加任务
		void push_task_type(ENUM_ACTOR atype, handle_pram& apram);

		//# 向当前进程所有actor广播消息
		void broadcast_task(handle_pram& apram);

		void run();

		//# 暂时挂起所有线程，已执行单步操作(热更数据表)
		void statrt_suspend_thread();
		void finish_suspend_thread();

		//# 获取actor数量
		int32_t actor_count();

		//# 获取actor stat 数据
		struct msg_actor
		{
			std::string m_actor_name;
			std::map<i16_area, std::vector<i32_actordataid>> m_actor;
			dprotocol(msg_actor, m_actor_name, m_actor)
		};
		struct msg_actor_stat
		{
			std::vector<msg_actor> m_vec;
			dprotocol(msg_actor_stat, m_vec)
		};
		void get_actor_stat(msg_actor_stat& adata);
	};

	//# 暂时挂起 actor_manage
	//# 自动调用
	//# 构造调用actor_manage.statrt_suspend_thread
	//# 析构调用actor_manage.finish_suspend_thread
	class actor_suspendthread
	{
		actor_suspendthread(const actor_suspendthread&) = delete;
		actor_suspendthread& operator=(const actor_suspendthread&) = delete;
	public:
		actor_suspendthread();
		~actor_suspendthread();
	};

	// 进程单利actor 自动注册协议与自动添加actor_manage
	template <typename T>
	T& actor_instance<T>::instance()
	{
		static T ltemp;
		static std::atomic<bool> lfirst = true;
		if (lfirst.exchange(false))
		{
			T* lptemp = &ltemp;
			actor_manage::instance().add_actor(&ltemp, [lptemp]()
				{
					lptemp->set_activity_stat(actor_stat_free);
					lptemp->init();
					lptemp->init_db_component(false);
				});
			ltemp.template init_rfun<T>();
			T::nregister();
		}
		return ltemp;
	}
}//namespace ngl