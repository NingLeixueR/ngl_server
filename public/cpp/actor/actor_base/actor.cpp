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

#include "actor/actor_base/actor_manage.h"
#include "actor/actor_base/ndbclient.h"
#include "actor/actor_base/actor.h"
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
		// Database actorparmbase.m_manage_dbclient
		// If database need tostart"whethersupportbroadcast"m_broadcast
		// Save data
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
		return m_list.empty() && m_hightlist.empty();
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

	void actor::push(handle_pram& apram)
	{
		int8_t highvalue = tprotocol::highvalue(apram.m_enum);
		lock_write(m_mutex);
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
		std::list<handle_pram> locallist;
		std::map<int32_t, std::list<handle_pram>> localhightlist;
		{
			lock_write(m_mutex);
			m_hightlist.swap(localhightlist);
			m_list.swap(locallist);
		}

		if (!localhightlist.empty())
		{
			for (auto& [_hight, _list] : localhightlist)
			{
				for (auto& _harm : _list)
				{
					ahandle(athreadid, _harm);
				}
			}
			localhightlist.clear();
		}
		
		auto llistcount = (int32_t)locallist.size();
		if (m_weight < llistcount || llistcount >= 0x7F)
		{
			log_error()->print("actor::actor_handle({}) {}:[weight:{}/count:{}]", athreadid, nguid(id_guid()), m_weight, locallist.size());
		}
		const time_t lbeg = localtime::gettimems();
		int32_t lweight = m_weight;
		while (--lweight >= 0 && !locallist.empty())
		{
			if (!m_release && localtime::timeout(lbeg, m_timeout))
			{
				break;
			}
			ahandle(athreadid, locallist.front());
			locallist.pop_front();
		}
		if (!locallist.empty())
		{
			lock_write(m_mutex);
			m_list.splice(m_list.begin(), locallist);
		}
	}

	bool actor::handle_broadcast(const message<np_actor_broadcast>& adata)
	{
		// # Save data
		if (manage_dbclient() != nullptr)
		{
			manage_dbclient()->save();
			manage_dbclient()->del();
		}
		// # Actor handle
		broadcast();
		return true;
	}

	bool actor::handle_close(const message<np_actor_close>&)
	{
		erase_actor();
		return true;
	}
}//namespace ngl