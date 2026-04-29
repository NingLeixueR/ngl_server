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
// File overview: Implements address table lookups and cross-node message forwarding.

#include "actor/actor_base/core/actor_base.h"
#include "actor/actor_base/core/nactortype.h"
#include "actor/actor_base/core/naddress.h"
#include "actor/actor_base/core/nguid.h"
#include "net/tcp/ntcp.h"

namespace ngl
{
	naddress::map_guidserver	naddress::m_actorserver;
	naddress::map_typeguid		naddress::m_actortypeserver;
	naddress::map_servernode	naddress::m_session;
	naddress::map_rolegateway	naddress::m_rolegateway;
	std::shared_mutex			naddress::m_actor_mutex;
	std::shared_mutex			naddress::m_session_mutex;
	std::shared_mutex			naddress::m_gateway_mutex;

	bool naddress::set_node(const nactornode& anode)
	{
		lock_write(m_session_mutex);
		nnode_session* lpsession = tools::findmap(m_session, anode.m_serverid);
		if (lpsession != nullptr)
		{
			// Reconnect/update path: keep the existing session slot and refresh node metadata.
			lpsession->m_node = anode;
			return false;
		}
		m_session.try_emplace(anode.m_serverid, anode);
		return true;
	}

	void naddress::print_address()
	{
		log_error()->print("actor address:{}", m_actorserver);
	}

	void naddress::print_address(const char* ainfo, i32_serverid aserverid, const nguid& aguid)
	{
		log_error()->print("{} actor address:{}-{}", ainfo, aserverid, aguid);
	}

	i32_sessionid naddress::sessionbyrole(i16_area aarea, i32_actordataid aroleid)
	{
		i64_actorid lactorrole = nguid::make(ACTOR_ROLE, aarea, aroleid);
		i32_serverid lserverid = serverid(lactorrole);
		if (lserverid == -1)
		{
			tools::no_core_dump();
			return -1;
		}
		return sessionid(lserverid);
	}

	bool naddress::forward(handle_pram& apram)
	{
		if (apram.m_forward != nullptr)
		{
			// The forwarding target is encoded into handle_pram by the protocol layer.
			apram.m_forward(apram);
		}
		return true;
	}

	void naddress::nosafe_actor_address_add(i32_serverid aserverid, i64_actorid adataid)
	{
		const nguid actor_guid(adataid);
		const i16_actortype ltype = nguid::type(adataid);
		// Maintain both direct lookup and reverse lookup by actor type.
		m_actorserver.insert_or_assign(actor_guid, aserverid);
		m_actortypeserver[ltype].insert(adataid);
#ifdef _DEBUG
		print_address("ADD", aserverid, adataid);
#endif		
	}

	void naddress::actor_address_add(i32_serverid aserverid, i64_actorid adataid)
	{
		lock_write(m_actor_mutex);
		nosafe_actor_address_add(aserverid, adataid);
	}

	void naddress::actor_address_add(i32_serverid aserverid, const std::vector<i64_actorid>& avec)
	{
		lock_write(m_actor_mutex);
		for (const i64_actorid lactorid : avec)
		{
			nosafe_actor_address_add(aserverid, lactorid);
		}
	}

	void naddress::nosafe_actor_address_del(i64_actorid adataid)
	{
		const nguid actor_guid(adataid);
		const i16_actortype ltype = nguid::type(adataid);
#ifdef _DEBUG
		auto lpserverid = tools::findmap(m_actorserver, adataid);
		if (lpserverid != nullptr)
		{
			print_address("DEL", *lpserverid, adataid);
		}
#endif
		m_actorserver.erase(actor_guid);
		auto itor = m_actortypeserver.find(ltype);
		if (itor != m_actortypeserver.end())
		{
			itor->second.erase(adataid);
		}
	}

	void naddress::actor_address_del(i64_actorid adataid)
	{
		lock_write(m_actor_mutex);
		nosafe_actor_address_del(adataid);
	}

	void naddress::actor_address_del(const std::vector<i64_actorid>& avec)
	{
		lock_write(m_actor_mutex);
		for (const i64_actorid lactorid : avec)
		{
			nosafe_actor_address_del(lactorid);
		}
	}

	void naddress::set_session(i32_serverid aserverid, i32_sessionid asession)
	{
		lock_write(m_session_mutex);
		nnode_session* lpsession = tools::findmap(m_session, aserverid);
		if (lpsession == nullptr)
		{
			log_error()->print("set_session(serverid:{},sessionid:{}) fail", aserverid, asession);
			return;
		}
		// Session ids are late-bound because the node record can exist before TCP connects.
		lpsession->m_session = asession;
	}

	i32_sessionid naddress::sessionid(i32_serverid aserverid)
	{
		lock_read(m_session_mutex);
		nnode_session* lpsession = tools::findmap(m_session, aserverid);
		if (lpsession == nullptr)
		{
			log_error()->print("sessionid(serverid:{}) fail", aserverid);
			return -1;
		}
		return lpsession->m_session;
	}

	i32_serverid naddress::serverid(const nguid& aguid)
	{
		lock_read(m_actor_mutex);
		i32_serverid* lpserverid = tools::findmap(m_actorserver, aguid);
		if (lpserverid == nullptr)
		{
			log_error()->print("sessionid(nguid:{}) fail", aguid);
			return -1;
		}
		return *lpserverid;
	}

	void naddress::serveridlist(ENUM_ACTOR atype, std::set<i32_serverid>& aservers)
	{
		lock_read(m_actor_mutex);
		const i16_actortype ltype = static_cast<i16_actortype>(atype);
		auto actor_guids = tools::findmap(m_actortypeserver, ltype);
		if (actor_guids == nullptr)
		{
			return;
		}

		for (const nguid& actor_guid : *actor_guids)
		{
			// Stay on the already-held shared lock instead of re-entering
			// serverid(), which would lock the same shared_mutex again.
			const i32_serverid* server_id = tools::findmap(m_actorserver, actor_guid);
			if (server_id != nullptr)
			{
				aservers.insert(*server_id);
			}
		}
	}

	void naddress::foreach(const foreach_callbackfun& afun)
	{
		lock_read(m_session_mutex);
		for (const auto& lpair : m_session)
		{
			if (afun(lpair.second) == false)
			{
				break;
			}
		}
	}

	void naddress::ergodic(const ergodic_callbackfun& afun)
	{
		// TODO: needs multi-lock strategy after lock split
		lock_read(m_actor_mutex);
		lock_read(m_session_mutex);
		lock_read(m_gateway_mutex);
		afun(m_actorserver, m_session, m_rolegateway);
	}

	naddress::map_guidserver naddress::get_actorserver_map()
	{
		lock_read(m_actor_mutex);
		return m_actorserver;
	}

	i32_serverid naddress::gatewayid(const nguid& aguid)
	{
		lock_read(m_gateway_mutex);
		const i32_serverid* lpserverid = tools::findmap(m_rolegateway, aguid);
		if (lpserverid == nullptr)
		{
			return -1;
		}
		return *lpserverid;
	}

	void naddress::gatewayid_add(const nguid& aguid, i32_serverid aserverid)
	{
		lock_write(m_gateway_mutex);
		m_rolegateway.insert_or_assign(aguid, aserverid);
	}

	void naddress::gatewayid_del(const nguid& aguid)
	{
		lock_write(m_gateway_mutex);
		m_rolegateway.erase(aguid);
	}

	void naddress::gatewayid(const std::set<nguid>& aactorset, std::set<i32_serverid>& aserverset)
	{
		lock_read(m_gateway_mutex);
		for (const nguid& lguid : aactorset)
		{
			const i32_serverid* lserverid = tools::findmap(m_rolegateway, lguid);
			if (lserverid == nullptr)
			{
				log_error()->print("naddress::gatewayid [{}] fail", lguid);
				continue;
			}
			// A set naturally de-duplicates gateways when many roles share one gateway process.
			aserverset.insert(*lserverid);
		}
	}
}//namespace ngl
