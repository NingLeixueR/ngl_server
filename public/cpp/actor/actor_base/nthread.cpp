/*
* Copyright (c) [2020-2025] NingLeixueR
* 
* ÏîÄ¿Ãû³Æ£ºngl_server
* ÏîÄ¿µØÖ·£ºhttps://github.com/NingLeixueR/ngl_server
* 
* ±¾ÎÄ¼þÊÇ ngl_server ÏîÄ¿µÄÒ»²¿·Ö£¬×ñÑ­ MIT ¿ªÔ´Ð­Òé·¢²¼¡£
* Äú¿ÉÒÔ°´ÕÕÐ­Òé¹æ¶¨×ÔÓÉÊ¹ÓÃ¡¢ÐÞ¸ÄºÍ·Ö·¢±¾ÏîÄ¿£¬°üÀ¨ÉÌÒµÓÃÍ¾£¬
* µ«Ðè±£ÁôÔ­Ê¼°æÈ¨ºÍÐí¿ÉÉùÃ÷¡£
* 
* Ðí¿ÉÏêÇé²Î¼ûÏîÄ¿¸ùÄ¿Â¼ÏÂµÄ LICENSE ÎÄ¼þ£º
* https://github.com/NingLeixueR/ngl_server/blob/main/LICENSE
*/
ï»¿#include "actor_manage.h"
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
				actor_manage::instance().push(lpactor, this);
			}
		}
	}
}//namespace ngl