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

	actor::~actor()
	{}

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
		return m_list.empty();
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
		monopoly_shared_lock(m_mutex);
		m_list.push_back(apram);
	}

	void actor::set_kcp(const handle_pram& aparm)
	{
		if (aparm.m_pack == nullptr
			|| aparm.m_pack->m_protocol != ENET_KCP
			|| is_single())
		{
			return;
		}
		set_kcpssion(aparm.m_pack->m_id);
	}

	bool actor::ahandle(i32_threadid athreadid, handle_pram& aparm)
	{
		//Try
		//{
			set_kcp(aparm);
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
			m_list.swap(m_locallist);
		}
		int32_t llistcount = (int32_t)m_locallist.size();
		if (m_weight < llistcount || llistcount >= 1000)
		{
			log_error()->print(
				"actor handle {}:[weight:{}/count:{}]",
				nguid(id_guid()), m_weight, m_locallist.size()
			);
		}
		time_t lbeg = localtime::gettimems();
		int32_t lcount = 0;
		int32_t lweight = m_weight;
		while (--lweight >= 0 && m_locallist.empty() != true)
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

	void actor::wait_ready(const nguid& aguid)
	{
		while (!actor_manage::instance().ready(aguid))
		{
			sleep::seconds(1);
		}
	}
}//namespace ngl