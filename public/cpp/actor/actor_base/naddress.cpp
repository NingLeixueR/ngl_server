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
#include "net.h"

namespace ngl
{
	naddress::map_guidserver	naddress::m_actorserver;
	naddress::map_typeguid		naddress::m_actortypeserver;
	naddress::map_servernode	naddress::m_session;
	naddress::map_rolegateway	naddress::m_rolegateway;

	bool naddress::set_node(const nactornode& anode)
	{
		actor_node_session* lpsession = tools::findmap(m_session, anode.m_serverid);
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
		i32_serverid lserverid = get_server(lactorrole);
		if (lserverid == -1)
		{
			tools::no_core_dump();
			return -1;
		}
		return get_session(lserverid);
	}

	bool naddress::forward(handle_pram& apram)
	{
		if (apram.m_forwardfun != nullptr)
		{
			apram.m_forwardfun(m_session, m_actorserver, apram);
		}
		return true;
	}

	void naddress::add_actor_address(i32_serverid aserverid, i64_actorid adataid)
	{
		nguid lguid(adataid);
		m_actorserver[lguid] = aserverid;
		m_actortypeserver[lguid.type()].insert(adataid);
#ifdef _DEBUG
		print_address("ADD", aserverid, adataid);
#endif
		
	}

	void naddress::add_actor_address(i32_serverid aserverid, const std::vector<i64_actorid>& avec)
	{
		for (const i64_actorid item : avec)
		{
			add_actor_address(aserverid, item);
#ifdef _DEBUG
			print_address("ADD", aserverid, item);
#endif
		}
	}

	void naddress::del_actor_address(i64_actorid adataid)
	{
		nguid lguid(adataid);
#ifdef _DEBUG
		auto itor = m_actorserver.find(adataid);
		if (itor != m_actorserver.end())
		{
			print_address("DEL", itor->second, itor->first);
		}
#endif
		m_actorserver.erase(lguid);
	}

	void naddress::del_actor_address(const std::vector<i64_actorid>& avec)
	{
		for (const i64_actorid item : avec)
		{
			del_actor_address(item);
		}
	}

	void naddress::set_session(i32_serverid aserverid, i32_sessionid asession)
	{
		actor_node_session* lpsession = tools::findmap(m_session, aserverid);
		if (lpsession == nullptr)
		{
			log_error()->print("set_session(serverid:{},sessionid:{}) fail", aserverid, asession);
			return;
		}
		lpsession->m_session = asession;
	}

	i32_sessionid naddress::get_session(i32_serverid aserverid)
	{
		actor_node_session* lpsession = tools::findmap(m_session, aserverid);
		if (lpsession == nullptr)
		{
			log_error()->print("get_session(serverid:{}) fail", aserverid);
			return -1;
		}
		return lpsession->m_session;
	}

	i32_serverid naddress::get_server(const nguid& aguid)
	{
		i32_serverid* lpserverid = tools::findmap(m_actorserver, aguid);
		if (lpserverid == nullptr)
		{
			log_error()->print("get_session(nguid:{}) fail", aguid);
			return -1;
		}
		return *lpserverid;
	}

	void naddress::get_serverlist(ENUM_ACTOR atype, std::set<i32_serverid>& avec)
	{
		std::set<nguid>* lset = tools::findmap(m_actortypeserver, (i16_actortype)atype);
		if (lset != nullptr)
		{
			for (const nguid& aguid : *lset)
			{
				avec.insert(get_server(aguid));
			}
		}
	}

	void naddress::foreach(const foreach_callbackfun& afun)
	{
		std::ranges::find_if(m_session, [&afun](const auto& apair)->bool
			{
				return afun(apair.second) == false;
			});
	}

	void naddress::ergodic(const ergodic_callbackfun& afun)
	{
		afun(m_actorserver, m_session);
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

	void naddress::add_gatewayid(const nguid& aguid, i32_serverid aserverid)
	{
		m_rolegateway[aguid] = aserverid;
	}

	void naddress::remove_gatewayid(const nguid& aguid)
	{
		m_rolegateway.erase(aguid);
	}

	void naddress::gatewayid(const std::set<nguid>& aactorset, std::set<i32_serverid>& aserverset)
	{
		for (const auto& aguid : aactorset)
		{
			const i32_serverid* lserverid = tools::findmap(m_rolegateway, aguid);
			if (lserverid == nullptr)
			{
				return;
			}
			aserverset.insert(*lserverid);
		}
	}
}//namespace ngl