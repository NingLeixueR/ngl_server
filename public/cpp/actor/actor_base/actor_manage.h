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
// File overview: Declares interfaces for actor base.

#pragma once

#include "actor/actor_logic/actor_client/actor_client.h"
#include "actor/actor_base/actor_base.h"
#include "actor/actor_base/naddress.h"
#include "actor/actor_base/nthread.h"
#include "tools/serialize/ndefine.h"
#include "tools/serialize/pack.h"
#include "tools/threadtools.h"

#include <deque>
#include <atomic>
#include <map>
#include <set>

namespace ngl
{
	class actor_manage
	{
	private:
		actor_manage(const actor_manage&) = delete;
		actor_manage& operator=(const actor_manage&) = delete;

		using ptrnthread = std::shared_ptr<nthread>;
		std::deque<ptrnthread>		m_workthreads;		// Thread
		std::deque<ptrnthread>		m_workthreadscopy;	// Thread( initialize copy, )
		bool						m_suspend = false;	// Whether
		std::deque<ptrnthread>		m_suspendthreads;	// Thread
		std::jthread				m_thread;			// Managethread
		i32_threadsize				m_threadnum = -1;	// Thread
		std::map<nguid, ptractor>	m_actorbyid;		// Indexactor
		std::map<nguid, ptractor>	m_actorbroadcast;	// Supportbroadcast actor
		std::deque<ptractor>		m_actorlist;		// Task actorlist
		std::set<i16_actortype>		m_actortype;		// Pack whichactortype
		std::map<nguid, std::function<void()>>			m_delactorfun;	// Deleteactorafterneed toexecute // (:delete actor state, after delete)
		std::map<ENUM_ACTOR, std::map<nguid, ptractor>> m_actorbytype;	// Typeindexactor

		std::shared_mutex			m_mutex;
		ngl::sem					m_sem;

		actor_manage();
		~actor_manage();

		// # Nosafe_ function table" lock", do not allow
		// # Guidgetactorinstance
		ptractor& nosafe_get_actor(const nguid& aguid);

		// # Guidgetactorinstance,if node tothis actorinstance, nodetypeget(actor_client/actor_server) guid, used toforwarding
		ptractor& nosafe_get_actorbyid(const nguid& aguid, handle_pram& apram);

		// # Toactorinstance task
		void nosafe_push_task_id(const ptractor& lpactor, handle_pram& apram);

		// # Actor_manage actorinstancehandletask threadinstance
		void run(std::stop_token astop);
	public:
		static actor_manage& instance()
		{
			static actor_manage ltemp;
			return ltemp;
		}

		// # Get
		nguid get_clientguid();

		// # Initialize set thread
		void init(i32_threadsize apthreadnum);

		// # Get actortype
		void get_type(std::vector<i16_actortype>& aactortype);

		// # Nodetypeget(actor_client/actor_server) guid
		nguid nodetypebyguid();

		// # MessageT after tospecifiedguid actor
		template <typename T, bool IS_SEND = true>
		inline void push_task_id(const nguid& aguid, std::shared_ptr<T>& apram)
		{
			handle_pram lparm = handle_pram::create<T, IS_SEND>(aguid, nguid::make(), apram);
			push_task_id(aguid, lparm);
		}

		// # Addactor
		bool add_actor(actor_base* apactor, const std::function<void()>& afun);

		// # Addactor
		bool add_actor(const ptractor& apactor, const std::function<void()>& afun);

		// # Removeactor
		void erase_actor(const nguid& aguid, const std::function<void()>& afun = nullptr);

		// # Whether actor
		bool is_have_actor(const nguid& aguid);

		// # Thread actoraddtom_actorlist
		void push(const ptractor& apactor, ptrnthread atorthread = nullptr);

		// # Toactorinaddtask
		void push_task_id(const nguid& aguid, handle_pram& apram);
		void push_task_id(const std::set<i64_actorid>& asetguid, handle_pram& apram);

		// # To type actorinaddtask
		void push_task_type(ENUM_ACTOR atype, handle_pram& apram);

		// # Tocurrent allactorbroadcastmessage
		void broadcast_task(handle_pram& apram);

		// # Allthread, execute ( datatable)
		void statrt_suspend_thread();
		void finish_suspend_thread();

		// # Getactor
		int32_t actor_count();

		// # Getactor stat data
		void get_actor_stat(msg_actor_stat& adata);
	};

	// # Actor_manage
	// # Automatically
	// # Actor_manage.statrt_suspend_thread
	// # Actor_manage.finish_suspend_thread
	class actor_suspend
	{
		actor_suspend(const actor_suspend&) = delete;
		actor_suspend& operator=(const actor_suspend&) = delete;
	public:
		actor_suspend();
		~actor_suspend();
	};

	// # Actor automaticallyregisterprotocolandautomaticallyaddactor_manage
	template <typename T>
	T& actor_instance<T>::instance()
	{
		static T ltemp;
		static std::atomic lfirst = true;
		if (lfirst.exchange(false))
		{
			T* lptemp = &ltemp;
			actor_manage::instance().add_actor(&ltemp, [lptemp]()
				{
					lptemp->set_activity_stat(actor_stat_free);
					lptemp->init();
					lptemp->init_db_component(false);
				}
			);
			ltemp.template init_rfun<T>();
			T::nregister();
		}
		return ltemp;
	}
}//namespace ngl

namespace ngl
{
	template <typename T, bool IS_SEND/* = true*/>
	void actor_base::send_actor(ENUM_ACTOR atype, const std::shared_ptr<T>& adata)
	{
		handle_pram lpram = handle_pram::create<T, IS_SEND>(nguid::make_self(atype), nguid::make(), adata);
		lpram.m_forwardtype = true;
		actor_manage::instance().push_task_type(atype, lpram);
	}

	template <typename T, bool IS_SEND/* = true*/>
	void actor_base::send_actor(const nguid& aguid, const nguid& arequestguid, const std::shared_ptr<T>& adata)
	{
		handle_pram lpram = handle_pram::create<T, IS_SEND>(aguid, arequestguid, adata);
		actor_manage::instance().push_task_id(aguid, lpram);
	}

	template <typename T, bool IS_SEND/* = true*/>
	void actor_base::send_actor(const nguid& aguid, const nguid& arequestguid, const std::shared_ptr<T>& adata, const std::function<void()>& afailfun)
	{
		handle_pram lpram = handle_pram::create<T, IS_SEND>(aguid, arequestguid, adata, afailfun);
		actor_manage::instance().push_task_id(aguid, lpram);
	}

	template <typename T, bool IS_SEND/* = true*/>
	void actor_base::send_actor(const std::set<i64_actorid>& asetguid, const nguid& arequestguid, const std::shared_ptr<T>& adata)
	{
		if (!asetguid.empty())
		{
			handle_pram lpram = handle_pram::create<T, IS_SEND>(asetguid, arequestguid, adata);
			actor_manage::instance().push_task_id(asetguid, lpram);
		}
	}

	template <typename T>
	void actor_base::send_client(const std::set<i64_actorid>& aids, const T& adata, ENET_PROTOCOL aprotocol/* = ENET_TCP*/)
	{
		auto pro = std::make_shared<np_actor_forward<T, forward_g2c<T>>>();
		pro->m_data.m_protocol = aprotocol;
		pro->m_data.m_data = adata;
		for (i64_actorid aactorid : aids)
		{
			nguid lguid(aactorid);
			pro->m_data.m_uid.push_back(lguid.actordataid());
			pro->m_data.m_area.push_back(lguid.area());
		}
		handle_pram lpram = handle_pram::create<T>(nguid::make(), nguid::make(), pro);
		actor_manage::instance().push_task_id(actorclient_guid(), lpram);
	}

	template <typename T>
	void actor_base::send_client(i64_actorid aid, const T& adata, ENET_PROTOCOL aprotocol/* = ENET_TCP*/)
	{
		std::set<i64_actorid> lids = { aid };
		send_client(lids, adata, aprotocol);
	}

	template <typename T>
	void actor_base::send_client(const std::vector<i64_actorid>& aids, const T& adata, ENET_PROTOCOL aprotocol/* = ENET_TCP*/)
	{
		std::set<i64_actorid> lids(aids.cbegin(), aids.cend());
		send_client(lids, adata, aprotocol);
	}

	template <typename T>
	void actor_base::send_client(const T& adata, ENET_PROTOCOL aprotocol/* = ENET_TCP*/)
	{
		const std::set<i32_serverid>& lgatewayids = sysconfig::gatewayids();
		if (!lgatewayids.empty())
		{
			auto pro = std::make_shared<np_actor_forward<T, forward_g2c<T>>>();
			pro->m_data.m_protocol = aprotocol;
			pro->m_data.m_data = adata;
			nguid lguid(nguid::make());
			pro->m_data.m_uid.push_back(lguid.actordataid());
			pro->m_data.m_area.push_back(lguid.area());
			send_server(lgatewayids, *pro, nguid::make(), nguid::make());
		}
	}

	template <typename T>
	void actor_base::send_clientbyarea(i16_area aarea, const T& adata, ENET_PROTOCOL aprotocol/* = ENET_TCP*/)
	{
		auto pro = std::make_shared<np_actor_forward<T, forward_g2c<T>>>();
		pro->m_data.m_protocol = aprotocol;
		pro->m_data.m_data = adata;
		ttab_servers::instance().foreach_server(GATEWAY, aarea, [&pro](const tab_servers* atab)
			{
				send_server(atab->m_id, *pro, nguid::make(), nguid::make());
			}
		);
	}
}//namespace ngl