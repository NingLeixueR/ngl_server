#pragma once

#include "actor_client.h"
#include "threadtools.h"
#include "actor_base.h"
#include "naddress.h"
#include "nthread.h"
#include "ndefine.h"
#include "pack.h"
#include "impl.h"

#include <list>
#include <map>
#include <set>

namespace ngl
{
	class actor_manage
	{
		actor_manage(const actor_manage&) = delete;
		actor_manage& operator=(const actor_manage&) = delete;

		struct impl_actor_manage;
		ngl::impl<impl_actor_manage> m_impl_actor_manage;

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
			def_jsonfunction(m_actor_name, m_actor)
		};
		struct msg_actor_stat
		{
			std::vector<msg_actor> m_vec;
			def_jsonfunction(m_vec)
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
		static bool first = true;
		if (first)
		{
			first = false;
			T* lptemp = &ltemp;
			actor_manage::instance().add_actor(&ltemp, [lptemp]()
				{
					lptemp->set_activity_stat(actor_stat_free);
					lptemp->init();
					lptemp->init_db_component(false);
					actor::create_log(lptemp->type());
				});
			ltemp.template init_rfun<T>();
			T::nregister();
		}
		return ltemp;
	}
}//namespace ngl