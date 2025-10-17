#pragma once

#include "threadtools.h"
#include "handle_pram.h"
#include "actor_base.h"

namespace ngl
{
	class nthread
	{
		nthread() = delete;
		nthread(const nthread&) = delete;
		nthread& operator=(const nthread&) = delete;

		i32_threadid		m_id;			// �߳�id
		ptractor			m_actor;		// ��ʱռ���̵߳�actor
		bool				m_isactivity;	// �߳��Ƿ��Ծ
		ngl::thread			m_thread;		// �߳�
		std::shared_mutex	m_mutex;		// ������
		ngl::sem			m_sem;			// �ź���

	public:
		nthread(i32_threadid aid);

		// # �����߳�id
		i32_threadid id();

		// # �Ƿ��Ծ
		bool isactivity();

		// # ���߳������actor
		void push(ptractor aactor);

	private:
		void run();
	};
}//namespace ngl



