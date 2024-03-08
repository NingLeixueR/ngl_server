#pragma once

#include "actor_client.h"
#include "actor_base.h"
#include "threadtools.h"
#include "naddress.h"
#include "nthread.h"
#include "pack.h"
#include "impl.h"
#include "net.h"

#include <list>
#include <map>
#include <set>

namespace ngl
{
	class actor_manage
	{
		struct impl_actor_manage;
		ngl::impl<impl_actor_manage> m_impl_actor_manage;

		actor_manage();
		~actor_manage();
	public:
		static actor_manage& getInstance()
		{
			static actor_manage ltemp;
			return ltemp;
		}

		// 初始化 设置工作线程数量
		void init(i32_threadsize apthreadnum);

		// 获取本进程存在的actor类型
		void get_type(std::vector<i16_actortype>& aactortype);

		// 添加actor
		bool add_actor(actor_base* apactor, const std::function<void()>& afun);
		bool add_actor(ptractor& apactor, const std::function<void()>& afun);

		// 移除actor
		void erase_actor_byid(const nguid& aguid, const std::function<void()>& afun = nullptr);

		// 是否存在某个actor
		bool is_have_actor(const nguid& aguid);

		// 工作线程将actor添加到m_actorlist
		void push(ptractor& apactor, nthread* atorthread = nullptr);

		// 向actor中添加任务
		void push_task_id(const nguid& aguid, handle_pram& apram, bool abool);

		// 向某个类型的actor中添加任务
		void push_task_type(ENUM_ACTOR atype, handle_pram& apram, bool aotherserver = false);

		// 向当前进程所有actor广播消息
		void broadcast_task(handle_pram& apram);

		// ## 暂时挂起所有线程，已执行单步操作(热更数据表)
		void statrt_suspend_thread();
		void finish_suspend_thread();
	};

	class actor_suspendthread
	{
	public:
		actor_suspendthread()
		{
			LogLocalInfo("start actor_suspendthread");
			actor_manage::getInstance().statrt_suspend_thread();
		}

		~actor_suspendthread()
		{
			Try
			{
				actor_manage::getInstance().finish_suspend_thread();
				LogLocalInfo("finish actor_suspendthread");
			}Catch;			
		}
	};

	template <typename T>
	T& actor_instance<T>::instance()
	{
		static T ltemp;
		static bool first = true;
		if (first)
		{
			Try
			{
				first = false;
				T* lptemp = &ltemp;
				actor_manage::getInstance().add_actor(&ltemp, [lptemp]()
				{
						Try
						{
							lptemp->set_activity_stat(actor_stat_free);
							lptemp->init();
							lptemp->init_db_component(false);
						}
						Catch;
				});
				ltemp.template init_rfun<T>();
				T::nregister();
			}Catch;
		}
		return ltemp;
	}
}//namespace ngl
