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
#include "actor_base.h"
#include "nactortype.h"
#include "naddress.h"
#include "nguid.h"
#include "ntcp.h"

namespace ngl
{
	naddress::map_guidserver	naddress::m_actorserver;
	naddress::map_typeguid		naddress::m_actortypeserver;
	naddress::map_servernode	naddress::m_session;
	naddress::map_rolegateway	naddress::m_rolegateway;

	bool naddress::set_node(const nactornode& anode)
	{
		nnode_session* lpsession = tools::findmap(m_session, anode.m_serverid);
		if (lpsession != nullptr)
		{
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
			apram.m_forward(apram);
		}
		return true;
	}

	void naddress::actor_address_add(i32_serverid aserverid, i64_actorid adataid)
	{
		nguid lguid(adataid);
		m_actorserver[lguid] = aserverid;
		m_actortypeserver[lguid.type()].insert(adataid);
#ifdef _DEBUG
		print_address("ADD", aserverid, adataid);
#endif		
	}

	void naddress::actor_address_add(i32_serverid aserverid, const std::vector<i64_actorid>& avec)
	{
		for (auto actorid : avec)
		{
			actor_address_add(aserverid, actorid);
		}
	}

	void naddress::actor_address_del(i64_actorid adataid)
	{
		nguid lguid(adataid);
#ifdef _DEBUG
		auto lpserverid = tools::findmap(m_actorserver, adataid);
		if (lpserverid != nullptr)
		{
			print_address("DEL", *lpserverid, adataid);
		}
#endif
		m_actorserver.erase(lguid);
	}

	void naddress::actor_address_del(const std::vector<i64_actorid>& avec)
	{
		for (const i64_actorid item : avec)
		{
			actor_address_del(item);
		}
	}

	void naddress::set_session(i32_serverid aserverid, i32_sessionid asession)
	{
		nnode_session* lpsession = tools::findmap(m_session, aserverid);
		if (lpsession == nullptr)
		{
			log_error()->print("set_session(serverid:{},sessionid:{}) fail", aserverid, asession);
			return;
		}
		lpsession->m_session = asession;
	}

	i32_sessionid naddress::sessionid(i32_serverid aserverid)
	{
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
		std::set<nguid>* lguids = tools::findmap(m_actortypeserver, (i16_actortype)atype);
		if (lguids != nullptr)
		{
			for (const nguid& aguid : *lguids)
			{
				aservers.insert(serverid(aguid));
			}
		}
	}

	void naddress::foreach(const foreach_callbackfun& afun)
	{
		std::ranges::find_if(m_session, [&afun](auto& apair)->bool
			{
				return afun(apair.second) == false;
			}
		);
	}

	void naddress::ergodic(const ergodic_callbackfun& afun)
	{
		afun(m_actorserver, m_session, m_rolegateway);
	}

	naddress::map_guidserver& naddress::get_actorserver_map()
	{
		return m_actorserver;
	}

	i32_serverid naddress::gatewayid(const nguid& aguid)
	{
		const i32_serverid* lpserverid = tools::findmap(m_rolegateway, aguid);
		if (lpserverid == nullptr)
		{
			return -1;
		}
		return *lpserverid;
	}

	void naddress::gatewayid_add(const nguid& aguid, i32_serverid aserverid)
	{
		m_rolegateway[aguid] = aserverid;
	}

	void naddress::gatewayid_del(const nguid& aguid)
	{
		m_rolegateway.erase(aguid);
	}

	void naddress::gatewayid(const std::set<nguid>& aactorset, std::set<i32_serverid>& aserverset)
	{
		for (auto& aguid : aactorset)
		{
			const i32_serverid* lserverid = tools::findmap(m_rolegateway, aguid);
			if (lserverid == nullptr)
			{
				log_error()->print("naddress::gatewayid [{}] fail", aguid);
				continue;
			}
			aserverset.insert(*lserverid);
		}
	}
}//namespace ngl