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

		//# ��ȡ
		nguid get_clientguid();

		//# ��ʼ�� ���ù����߳�����
		void init(i32_threadsize apthreadnum);

		//# ��ȡ�����̴��ڵ�actor����
		void get_type(std::vector<i16_actortype>& aactortype);

		//# actor����״̬(�����Ҫ����db��db�������)
		bool ready(const nguid& aguid);

		void set_ready(const nguid& aguid);

		//# ���actor
		bool add_actor(actor_base* apactor, const std::function<void()>& afun);

		//# ���actor
		bool add_actor(const ptractor& apactor, const std::function<void()>& afun);

		//# �Ƴ�actor
		void erase_actor(const nguid& aguid, const std::function<void()>& afun = nullptr);

		//# �Ƿ����ĳ��actor
		bool is_have_actor(const nguid& aguid);

		//# �����߳̽�actor��ӵ�m_actorlist
		void push(const ptractor& apactor, nthread* atorthread = nullptr);

		//# ��actor���������
		void push_task_id(const nguid& aguid, handle_pram& apram);
		void push_task_id(const std::set<i64_actorid>& asetguid, handle_pram& apram);

		//# ��ĳ�����͵�actor���������
		void push_task_type(ENUM_ACTOR atype, handle_pram& apram);

		//# ��ǰ��������actor�㲥��Ϣ
		void broadcast_task(handle_pram& apram);

		//# ��ʱ���������̣߳���ִ�е�������(�ȸ����ݱ�)
		void statrt_suspend_thread();
		void finish_suspend_thread();

		//# ��ȡactor����
		int32_t actor_count();

		//# ��ȡactor stat ����
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

	//# ��ʱ���� actor_manage
	//# �Զ�����
	//# �������actor_manage.statrt_suspend_thread
	//# ��������actor_manage.finish_suspend_thread
	class actor_suspendthread
	{
		actor_suspendthread(const actor_suspendthread&) = delete;
		actor_suspendthread& operator=(const actor_suspendthread&) = delete;
	public:
		actor_suspendthread();
		~actor_suspendthread();
	};

	// ���̵���actor �Զ�ע��Э�����Զ����actor_manage
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