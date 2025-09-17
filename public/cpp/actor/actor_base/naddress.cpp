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
	std::map<i64_actorid, std::map<int64_t, std::function<void()>>> naddress::m_nguidfun;

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
		log_error()->print("#actor_address [[{}]->[{}]]", lguid, aserverid);

		auto itor = m_nguidfun.find(adataid);
		if (itor != m_nguidfun.end())
		{
			for (const auto& item : itor->second)
			{
				item.second();
			}
			m_nguidfun.erase(adataid);
		}
	}

	void naddress::add_actor_address(i32_serverid aserverid, const std::vector<i64_actorid>& avec)
	{
		for (const i64_actorid item : avec)
		{
			add_actor_address(aserverid, item);
		}
		//#ifdef _DEBUG
		//		print_address();
		//#endif
	}

	void naddress::del_actor_address(i64_actorid adataid)
	{
		nguid lguid(adataid);
		m_actorserver.erase(lguid);
	}

	void naddress::del_actor_address(const std::vector<i64_actorid>& avec)
	{
		for (const i64_actorid item : avec)
			del_actor_address(item);
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

	void naddress::server_actor_send(const nguid& aguid, const std::function<void()>& afun)
	{
		if (aguid.type() == ACTOR_LOG)
		{
			return;
		}
		enum
		{
			e_server_actor_send_count = 100,
		};
		int32_t ltemp[2] = { 0 };
		ltemp[0] = localtime::gettime();
		std::map<int64_t, std::function<void()>>& lmap = m_nguidfun[aguid];
		if (lmap.size() > e_server_actor_send_count)
		{
			lmap.erase(lmap.begin());
		}

		for (int i = 0; i < e_server_actor_send_count; ++i)
		{
			ltemp[1] = i;
			int64_t lvalue = *(int64_t*)ltemp;
			if (!lmap.contains(lvalue))
			{
				lmap[lvalue] = afun;
				break;
			}
		}
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