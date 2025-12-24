// clang-format off
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
// clang-format on
#include "actor_manage.h"
#include "ndbclient.h"
#include "actor.h"
#include "net.h"

namespace ngl
{
	actor::actor(const actorparm& aparm) :
		actor_base(aparm.m_parm)
		, m_weight(aparm.m_weight)
		, m_timeout(aparm.m_timeout)
	{
		if (aparm.m_weight <= 0 || m_timeout <= 0)
		{
			tools::no_core_dump();
		}
		// 检查数据库依赖actorparmbase.m_manage_dbclient
		// 如果依赖数据库那么需要开启"是否支持广播"m_broadcast
		// 以便顺利保存数据修改
		if (aparm.m_parm.m_manage_dbclient && !aparm.m_broadcast)
		{
			log_error()->print("actorparm fail [m_parm.m_manage_dbclient && !m_broadcast]");
			actorparm lparm = aparm;
			lparm.m_broadcast = true;
			set_broadcast(true);
		}
		else
		{
			set_broadcast(aparm.m_broadcast);
		}
	}

	void actor::release()
	{
		monopoly_shared_lock(m_mutex);
		m_release = true;
		actor_handle(0x7fffffff);
		save();
		m_list.clear();
	}

	bool actor::list_empty()
	{
		monopoly_shared_lock(m_mutex);
		return m_list.empty() && m_hightlist.empty();
	}

	actor_stat actor::get_activity_stat()
	{
		monopoly_shared_lock(m_mutex);
		return m_stat;
	}

	void actor::set_activity_stat(actor_stat astat)
	{
		monopoly_shared_lock(m_mutex);
		m_stat = astat;
	}

	void actor::push(handle_pram& apram)
	{
		int8_t highvalue = 0;
		const tprotocol::info* lpinfo = tprotocol::get(apram.m_enum);
		if (lpinfo != nullptr)
		{
			highvalue = lpinfo->m_highvalue;
		}
		monopoly_shared_lock(m_mutex);
		if (highvalue <= 0)
		{
			m_list.push_back(apram);
		}
		else
		{
			m_hightlist[highvalue].push_back(apram);
		}
	}

	bool actor::ahandle(i32_threadid athreadid, handle_pram& aparm)
	{
		//Try
		//{
			nrfunbase* lprfun = m_actorfun;
			if (lprfun == nullptr)
			{
				tools::no_core_dump();
				return false;
			}
			if (lprfun->handle_switch(this, athreadid, aparm))
			{
				return true;
			}
			lprfun->notfindfun(this, athreadid, aparm);
			return true;
		//}Catch
		//return false;
	}

	void actor::actor_handle(i32_threadid athreadid)
	{
		{
			monopoly_shared_lock(m_mutex);
			m_hightlist.swap(m_localhightlist);
			m_list.swap(m_locallist);		
		}

		if (!m_localhightlist.empty())
		{
			for (auto itor = m_localhightlist.rbegin(); itor != m_localhightlist.rend(); ++itor)
			{
				for (auto itor2 = itor->second.begin(); itor2 != itor->second.end(); ++itor2)
				{
					ahandle(athreadid, *itor2);
				}
			}
			m_localhightlist.clear();
		}
		
		auto llistcount = (int32_t)m_locallist.size();
		if (m_weight < llistcount || llistcount >= 0x7F)
		{
			log_error()->print("actor::actor_handle({}) {}:[weight:{}/count:{}]", athreadid, nguid(id_guid()), m_weight, m_locallist.size());
		}
		time_t lbeg = localtime::gettimems();
		int32_t lcount = 0;
		int32_t lweight = m_weight;
		while (--lweight >= 0 && !m_locallist.empty())
		{
			if (m_release == false && localtime::gettimems() - lbeg > m_timeout)
			{
				break;
			}
			handle_pram& lparm = m_locallist.front();
			ahandle(athreadid, lparm);
			m_locallist.pop_front();
			++lcount;
		}
		if (!m_locallist.empty())
		{
			monopoly_shared_lock(m_mutex);
#ifdef STL_MESSAGELIST
			m_list.insert(m_list.begin(), m_locallist.begin(), m_locallist.end());
#else
			m_list.push_front(m_locallist);
#endif
		}
	}

	bool actor::handle(const message<np_actor_broadcast>& adata)
	{
		// # 保存数据
		if (get_actor_manage_dbclient() != nullptr)
		{
			get_actor_manage_dbclient()->save();
			get_actor_manage_dbclient()->del();
		}
		// # actor派生类自定义处理逻辑
		broadcast();
		return true;
	}

	bool actor::handle(const message<np_actor_close>&)
	{
		erase_actor();
		return true;
	}
}//namespace ngl