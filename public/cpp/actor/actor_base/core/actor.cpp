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
// File overview: Implements actor construction, message queue drain, and priority scheduling.

#include "actor/actor_base/core/actor_manage.h"
#include "actor/actor_base/core/ndbclient.h"
#include "actor/actor_base/core/actor.h"
#include "net/tcp/ntcp.h"
#include "tools/tools/tools_sys.h"

#include <format>

namespace ngl
{
	actor::actor(const actorparm& aparm) :
		actor_base(aparm.m_parm)
		, m_weight(aparm.m_weight)
		, m_timeout(aparm.m_timeout)
	{
		m_normalstat.m_warncnt = aparm.m_normalwarn;
		m_normalstat.m_hight = false;
		m_highstat.m_warncnt = aparm.m_highwarn;
		m_highstat.m_hight = true;
		if (aparm.m_weight <= 0 || m_timeout <= 0 || aparm.m_normalwarn < 0 || aparm.m_highwarn < 0)
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
			// Reset warning state so a recycled actor does not carry stale counters.
			m_normalstat.m_lastms = 0;
			m_highstat.m_lastms = 0;
			m_normalstat.m_skipcnt = 0;
			m_highstat.m_skipcnt = 0;
			m_normalstat.m_cnt = 0;
			m_highstat.m_cnt = 0;
		}
	}

	bool actor::list_empty()
	{
		lock_read(m_mutex);
		auto lhight = ready().hightlevel_ready();
		if(!lhight.has_value())
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
				if (m_hightlist.begin()->first < *lhight)
				{// Non-empty: priority below threshold, ready to run.
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

	std::optional<int32_t> actor::hight_value()
	{
		if (m_hightlist.empty())
		{
			return std::nullopt;
		}
		return m_hightlist.begin()->first;
	}

	bool actor::push(handle_pram& apram)
	{
		auto lhigh = tprotocol::highvalue(apram.m_enum);
		bool lret = false;
		bool lsendmail = false;
		queue_warn lqwarn;
		const time_t lnow = tools::time::gettimems();
		{
			lock_write(m_mutex);
			if (lhigh.has_value() && *lhigh > 0)
			{
				// High-priority: insert into the bucket for this priority level.
				// A newly arrived priority lower than the current head may advance readiness.
				auto loldhigh = hight_value();
				m_hightlist[*lhigh].emplace_back(apram);
				++m_highstat.m_cnt;
				lret = !loldhigh.has_value() || *lhigh < loldhigh;
				lsendmail = m_highstat.iswarn(lqwarn, lnow, e_warngap);
			}
			else
			{
				// Normal-priority: append to FIFO tail and bump the depth counter.
				m_list.emplace_back(apram);
				++m_normalstat.m_cnt;
				lret = ready().is_ready();
				lsendmail = m_normalstat.iswarn(lqwarn, lnow, e_warngap);
			}
		}

		// Logging and mail are done outside the lock to avoid blocking other producers.
		if (lsendmail)
		{
			std::string lmessage;
			lmessage = std::format(
				"actor queue warn [{}] actor:{} protocol:{} request:{} count:{}/{} suppressed:{}"
				, lqwarn.m_hight ? "high" : "normal"
				, id_guid()
				, apram.m_enum
				, apram.m_requestactor
				, lqwarn.m_cnt
				, lqwarn.m_warncnt
				, lqwarn.m_skipcnt
			);
			log_error()->print("{}", lmessage);
			tools::send_mail(lmessage, false)();
		}
		return lret;
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
		// --- Phase 1: drain high-priority messages ---
		// Swap the entire high-priority map into a local copy so handlers can freely
		// enqueue new messages without contending on the actor mutex.
		std::map<int32_t, std::list<handle_pram>> localhightlist;
		{
			lock_write(m_mutex);
			m_hightlist.swap(localhightlist);
			m_highstat.m_cnt = 0;
		}

		auto lvalue = ready().hightlevel_ready();

		if (!localhightlist.empty())
		{
			for(auto itor = localhightlist.begin();itor != localhightlist.end();)
			{
				// Dispatch buckets whose priority is more urgent than the readiness gate.
				if(!lvalue.has_value() || *lvalue > itor->first)
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

			// Return unconsumed high-priority messages to the shared map and restore the depth counter.
			lock_write(m_mutex);
			for (auto& [key, value] : localhightlist)
			{
				auto& lmap = m_hightlist[key];
				m_highstat.m_cnt += static_cast<int32_t>(value.size());
				lmap.splice(lmap.begin(), std::move(value));
			}
		}

		if (!ready().is_ready())
		{
			return false;
		}
		// --- Phase 2: drain normal-priority messages ---
		// Same swap pattern: take ownership of the list, reset the depth counter,
		// then process up to m_weight messages within the m_timeout budget.
		std::list<handle_pram> locallist;
		{
			lock_write(m_mutex);
			m_list.swap(locallist);
			m_normalstat.m_cnt = 0;
		}
		
		auto llistcount = (int32_t)locallist.size();
		if (m_weight < llistcount || llistcount >= 0x7F)
		{
			if (type() != ACTOR_LOG)
			{
				log_error()->print("actor::actor_handle({}) {}:[weight:{}/count:{}]", athreadid, nguid(id_guid()), m_weight, locallist.size());
			}
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
			// Splice unprocessed messages back to the front to preserve FIFO ordering,
			// and restore the depth counter for accurate overflow detection.
			m_normalstat.m_cnt += static_cast<int32_t>(locallist.size());
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
