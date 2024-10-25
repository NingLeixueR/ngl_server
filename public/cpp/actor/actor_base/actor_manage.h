#pragma once

#include "actor_client.h"
#include "threadtools.h"
#include "define_json.h"
#include "actor_base.h"
#include "naddress.h"
#include "nthread.h"
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
		static actor_manage& getInstance()
		{
			static actor_manage ltemp;
			return ltemp;
		}

		//# 初始化 设置工作线程数量
		void init(i32_threadsize apthreadnum);

		//# 获取本进程存在的actor类型
		void get_type(std::vector<i16_actortype>& aactortype);

		//# 添加actor
		bool add_actor(actor_base* apactor, const std::function<void()>& afun);

		//# 添加actor
		bool add_actor(const ptractor& apactor, const std::function<void()>& afun);

		//# 移除actor
		void erase_actor_byid(const nguid& aguid, const std::function<void()>& afun = nullptr);

		//# 是否存在某个actor
		bool is_have_actor(const nguid& aguid);

		//# 工作线程将actor添加到m_actorlist
		void push(const ptractor& apactor, nthread* atorthread = nullptr);

		//# 向actor中添加任务
		void push_task_id(const nguid& aguid, handle_pram& apram, bool abool);

		//# 向某个类型的actor中添加任务
		void push_task_type(ENUM_ACTOR atype, handle_pram& apram, bool aotherserver = false);

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
			jsonfunc("actor_name", m_actor_name, "actor", m_actor)
		};
		struct msg_actor_stat
		{
			std::vector<msg_actor> m_vec;
			jsonfunc("vec", m_vec)
		};
		void actor_stat(msg_actor_stat& adata);
	};

	class actor_suspendthread
	{
	public:
		actor_suspendthread()
		{
			log_info()->print("start actor_suspendthread");
			actor_manage::getInstance().statrt_suspend_thread();
		}

		~actor_suspendthread()
		{
			Try
			{
				actor_manage::getInstance().finish_suspend_thread();
				log_info()->print("finish actor_suspendthread");
			}Catch
		}
	};

	template <typename T>
	T& actor_instance<T>::instance()
	{
		static T ltemp;
		if (static bool first = true; first)
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
							if (lptemp->type() != ngl::ACTOR_LOG)
							{
								nlogactor lnlogactor(lptemp->type(), ngl::ELOG_LOCAL);
								ngl::actor_base::create(
									ngl::ACTOR_LOG, tab_self_area, lnlogactor.m_value32
								);
							}
						}
						Catch
				});
				ltemp.template init_rfun<T>();
				T::nregister();
			}Catch
		}
		return ltemp;
	}
}//namespace ngl
