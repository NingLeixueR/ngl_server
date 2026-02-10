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
#include "actor_manage.h"
#include "nthread.h"

namespace ngl
{
	nthread::nthread(i32_threadid aid) :
		m_id(aid),
		m_actor(nullptr),
		m_isactivity(false),
		m_thread(&nthread::run, this)
	{
	}

	i32_threadid nthread::id()
	{
		return m_id;
	}

	bool nthread::isactivity()
	{
		monopoly_shared_lock(m_mutex);
		return m_isactivity;
	}

	void nthread::push(ptractor aactor)
	{
		monopoly_shared_lock(m_mutex);
		m_actor = aactor;
		m_isactivity = false;
		m_sem.post();
	}

	void nthread::run()
	{
		ptractor lpactor = nullptr;
		while (true)
		{
			m_sem.wait();
			{
				monopoly_shared_lock(m_mutex);
				lpactor = m_actor;
			}
			if (lpactor != nullptr)
			{
				if (!lpactor->list_empty())
				{
					lpactor->actor_handle(m_id);
					{
						monopoly_shared_lock(m_mutex);
						m_actor = nullptr;
						m_isactivity = false;
					}
				}
				actor_manage::instance().push(lpactor, shared_from_this());
			}
		}
	}
}//namespace ngl