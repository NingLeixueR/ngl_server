/*
* Copyright (c) [2020-2025] NingLeixueR
* 
* Project name: ngl_server
* Project URL: https://github.com/NingLeixueR/ngl_server
* 
* This file is part of the ngl_server project and is distributed under the MIT License.
* You may use, modify, and distribute this project under the license, including commercial use,
* but you must retain the original copyright and license notice.
* 
* For license details, see the LICENSE file in the project root:
* https://github.com/NingLeixueR/ngl_server/blob/main/LICENSE
*/
// File overview: Implements logic for actor base.

#include "actor/actor_base/core/actor_manage.h"
#include "actor/actor_base/core/ndbclient.h"
#include "actor/actor_base/core/actor.h"
#include "net/tcp/ntcp.h"

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
		// DB-backed actors must be included in the broadcast tick so they can flush
		// pending changes during the maintenance pass.
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
		{
			lock_write(m_mutex);
			m_release = true;
		}
		// Drain the remaining queue synchronously before clearing containers so shutdown
		// follows the same ordering rules as normal scheduling.
		actor_handle(0x7fffffff);
		save();
		{
			lock_write(m_mutex);
			m_list.clear();
			m_hightlist.clear();
		}
	}

	bool actor::list_empty()
	{
		
		lock_read(m_mutex);
		int32_t lhight = ready().hightlevel_ready();
		if(lhight == -1)
		{
			return m_list.empty() && m_hightlist.empty();
		}
		else
		{
			if (m_hightlist.empty())
			{
				return true;
			}
			else
			{
				if (m_hightlist.begin()->first < lhight)
				{//列表不为空因为有可执行的
					return false;
				}
				else
				{
					return true;
				}
			}
		}
	}

	bool actor::high_empty()
	{
		lock_read(m_mutex);
		return m_hightlist.empty();
	}

	actor_stat actor::activity_stat()
	{
		lock_read(m_mutex);
		return m_stat;
	}

	void actor::set_activity_stat(actor_stat astat)
	{
		lock_write(m_mutex);
		m_stat = astat;
	}

	int32_t actor::hight_value()
	{
		if (m_hightlist.empty())
		{
			return -1;
		}
		return m_hightlist.begin()->first;
	}

	bool actor::push(handle_pram& apram)
	{
		int32_t lhigh = tprotocol::highvalue(apram.m_enum);
		lock_write(m_mutex);
		if (lhigh <= 0)
		{
			// Default traffic is processed in FIFO order.
			m_list.emplace_back(apram);
			return ready().is_ready();
		}
		else
		{
			// Lower privilege values are allowed to pass earlier readiness gates.
			const int32_t loldhigh = hight_value();
			m_hightlist[lhigh].emplace_back(apram);
			return loldhigh == -1 || lhigh < loldhigh;
		}
	}

	bool actor::ahandle(i32_threadid athreadid, handle_pram& aparm)
	{
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
		// Report unknown messages through the registration layer so debugging stays centralized.
		lprfun->notfindfun(this, athreadid, aparm);
		return true;
	}

	bool actor::actor_handle(i32_threadid athreadid)
	{
		std::map<int32_t, std::list<handle_pram>> localhightlist;
		{
			lock_write(m_mutex);
			// Move work out of the shared queues so handlers can enqueue more messages without
			// holding the actor mutex for the whole batch.
			m_hightlist.swap(localhightlist);
		}

		int32_t lvalue = ready().hightlevel_ready();

		if (!localhightlist.empty())
		{
			for(auto itor = localhightlist.begin();itor != localhightlist.end();)
			{
				// Lower privilege values are allowed to pass earlier readiness gates.
				if (lvalue == -1 || lvalue > itor->first)
				{
					for (auto& lharm : itor->second)
					{
						ahandle(athreadid, lharm);
					}
					itor = localhightlist.erase(itor);
				}
				else
				{
					break;
				}
			}

			lock_write(m_mutex);
			for (auto& [key, value] : localhightlist)
			{
				auto& lmap = m_hightlist[key];
				lmap.splice(lmap.begin(), std::move(value));
			}
		}

		if (!ready().is_ready())
		{
			return false;
		}
		std::list<handle_pram> locallist;
		{
			lock_write(m_mutex);
			m_list.swap(locallist);
		}
		auto llistcount = (int32_t)locallist.size();
		if (m_weight < llistcount || llistcount >= 0x7F)
		{
			log_error()->print("actor::actor_handle({}) {}:[weight:{}/count:{}]", athreadid, nguid(id_guid()), m_weight, locallist.size());
		}
		const time_t lbeg = tools::time::gettimems();
		int32_t lweight = m_weight;
		while (--lweight >= 0 && !locallist.empty())
		{
			// During shutdown we ignore the time budget and finish draining immediately.
			if (!m_release && tools::time::timeout(lbeg, m_timeout))
			{
				break;
			}
			ahandle(athreadid, locallist.front());
			locallist.pop_front();
		}
		if (!locallist.empty())
		{
			lock_write(m_mutex);
			// Unprocessed normal-priority messages go back to the front so ordering is preserved.
			m_list.splice(m_list.begin(), locallist);
		}
		return true;
	}

	bool actor::handle_broadcast([[maybe_unused]] const message<np_actor_broadcast>& adata)
	{
		// Persist first so broadcast() can assume DB-backed state is already synchronized.
		if (manage_dbclient() != nullptr)
		{
			manage_dbclient()->save();
			manage_dbclient()->del();
		}
		broadcast();
		return true;
	}

	bool actor::handle_close(const message<np_actor_close>&)
	{
		erase_actor();
		return true;
	}
}//namespace ngl
