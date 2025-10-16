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

		//// ---- �߳����
		std::list<nthread*>	m_workthread;		// �����߳�
		bool				m_suspend = false;	// �Ƿ����
		std::list<nthread*>	m_suspendthread;	// ����Ĺ����߳�
		std::jthread		m_thread;			// �����߳�
		i32_threadsize		m_threadnum = -1;	// �����߳�����

		ngl_lockinit;

		// # ����actor
		std::map<nguid, ptractor>							m_actorbyid;
		// # ֧�ֹ㲥��actor
		std::map<nguid, ptractor>							m_actorbroadcast;
		// # ����������actor
		std::map<ENUM_ACTOR, std::map<nguid, ptractor>>		m_actorbytype;
		// # �������actor�б�
		std::list<ptractor>									m_actorlist;
		// # ������Щactortype
		std::set<i16_actortype>								m_actortype;
		// # ɾ��actor����Ҫִ�еĲ���(�ӳٲ���:ɾ����˲��actor��������״̬,�ȴ���ع�����ɾ��)
		std::map<nguid, std::function<void()>>				m_delactorfun;
		// # actor����״̬(�����Ҫ����db��db�������)
		std::set<nguid>										m_ready;

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

		nguid nodetypebyguid();

		ptractor& nosafe_get_actor(const nguid& aguid);

		ptractor& nosafe_get_actorbyid(const nguid& aguid, handle_pram& apram);

		// # nosafe_��ͷ�ĺ�������"�ڲ�����δ����"���������������
		void nosafe_push_task_id(const ptractor& lpactor, handle_pram& apram);

		template <typename T, bool IS_SEND = true>
		inline void push_task_id(const nguid& aguid, std::shared_ptr<T>& apram)
		{
			handle_pram lparm = handle_pram::create<T, IS_SEND>(aguid, nguid::make(), apram);
			push_task_id(aguid, lparm);
		}

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

		void run();

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
			dprotocol(msg_actor, m_actor_name, m_actor)
		};
		struct msg_actor_stat
		{
			std::vector<msg_actor> m_vec;
			dprotocol(msg_actor_stat, m_vec)
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