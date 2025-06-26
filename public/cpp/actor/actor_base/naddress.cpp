#include "actor_base.h"
#include "nactortype.h"
#include "naddress.h"
#include "nguid.h"
#include "net.h"

namespace ngl
{
	struct impl_actor_address
	{
		impl_actor_address() = delete;
		impl_actor_address(const impl_actor_address&) = delete;
		impl_actor_address& operator=(const impl_actor_address&) = delete;
	private:
		static naddress::map_guidserver		m_actorserver;
		static naddress::map_typeguid		m_actortypeserver;
		static naddress::map_servernode		m_session;
		static naddress::map_rolegateway	m_rolegateway;
	public:
		static bool set_node(const nactornode& anode)
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

		static void print_address()
		{
			log_error()->print("actor address:{}", m_actorserver);
		}

		static i32_sessionid sessionbyrole(i16_area aarea, i32_actordataid aroleid)
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

		static bool forward(handle_pram& apram)
		{
			if (apram.m_forwardfun != nullptr)
			{
				apram.m_forwardfun(m_session, m_actorserver, apram);
			}
			return true;
		}

		static void add_actor_address(i32_serverid aserverid, i64_actorid adataid)
		{
			nguid lguid(adataid);
			m_actorserver[lguid] = aserverid;
			m_actortypeserver[lguid.type()].insert(adataid);
		}

		static void del_actor_address(i64_actorid adataid)
		{
			nguid lguid(adataid);
			m_actorserver.erase(lguid);
		}

		static void set_session(i32_serverid aserverid, i32_sessionid asession)
		{
			actor_node_session* lpsession = tools::findmap(m_session, aserverid);
			if (lpsession == nullptr)
			{
				log_error()->print("set_session(serverid:{},sessionid:{}) fail", aserverid, asession);
				return;
			}
			lpsession->m_session = asession;
		}

		static i32_sessionid get_session(i32_serverid aserverid)
		{
			actor_node_session* lpsession = tools::findmap(m_session, aserverid);
			if (lpsession == nullptr)
			{
				log_error()->print("get_session(serverid:{}) fail", aserverid);
				return -1;
			}
			return lpsession->m_session;
		}

		static i32_serverid get_server(const nguid& aguid)
		{
			i32_serverid* lpserverid = tools::findmap(m_actorserver, aguid);
			if (lpserverid == nullptr)
			{
				log_error()->print("get_session(nguid:{}) fail", aguid);
				return -1;
			}
			return *lpserverid;
		}

		static void get_serverlist(ENUM_ACTOR atype, std::set<i32_serverid>& avec)
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

		static void foreach(const naddress::foreach_callbackfun& afun)
		{
			std::ranges::find_if(m_session, [&afun](const auto& apair)->bool
				{
					return afun(apair.second) == false;
				});
		}

		static void ergodic(const naddress::ergodic_callbackfun& afun)
		{
			afun(m_actorserver, m_session);
		}

		static naddress::map_guidserver& get_actorserver_map()
		{
			return m_actorserver;
		}

		static i32_serverid gatewayid(const nguid& aguid)
		{
			const i32_serverid* lpserverid = tools::findmap(m_rolegateway, aguid);
			if (lpserverid == nullptr)
			{
				return -1;
			}
			return *lpserverid;
		}

		static void add_gatewayid(const nguid& aguid, i32_serverid aserverid)
		{
			m_rolegateway[aguid] = aserverid;
		}

		static void remove_gatewayid(const nguid& aguid)
		{
			m_rolegateway.erase(aguid);
		}

		static void gatewayid(const std::set<nguid>& aactorset, std::set<i32_serverid>& aserverset)
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
	};

	naddress::map_guidserver	impl_actor_address::m_actorserver;
	naddress::map_typeguid		impl_actor_address::m_actortypeserver;
	naddress::map_servernode	impl_actor_address::m_session;
	naddress::map_rolegateway	impl_actor_address::m_rolegateway;

	bool naddress::set_node(const nactornode& anode)
	{
		return impl_actor_address::set_node(anode);
	}

	void naddress::print_address()
	{
		impl_actor_address::print_address();
	}

	i32_sessionid naddress::sessionbyrole(i16_area aarea, i32_actordataid aroleid)
	{
		return impl_actor_address::sessionbyrole(aarea, aroleid);
	}

	bool naddress::forward(handle_pram& apram)
	{
		return impl_actor_address::forward(apram);
	}

	void naddress::add_actor_address(i32_serverid aserverid, i64_actorid adataid)
	{
		impl_actor_address::add_actor_address(aserverid, adataid);
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
		impl_actor_address::del_actor_address(adataid);
	}

	void naddress::del_actor_address(const std::vector<i64_actorid>& avec)
	{
		for (const i64_actorid item : avec)
			del_actor_address(item);
	}

	void naddress::set_session(i32_serverid aserverid, i32_sessionid asession)
	{
		impl_actor_address::set_session(aserverid, asession);
	}

	i32_sessionid naddress::get_session(i32_serverid aserverid)
	{
		return impl_actor_address::get_session(aserverid);
	}

	i32_serverid naddress::get_server(const nguid& aguid)
	{
		return impl_actor_address::get_server(aguid);
	}

	void naddress::get_serverlist(ENUM_ACTOR atype, std::set<i32_serverid>& avec)
	{
		impl_actor_address::get_serverlist(atype, avec);
	}

	void naddress::foreach(const foreach_callbackfun& afun)
	{
		impl_actor_address::foreach(afun);
	}

	void naddress::ergodic(const ergodic_callbackfun& afun)
	{
		impl_actor_address::ergodic(afun);
	}

	naddress::map_guidserver& naddress::get_actorserver_map()
	{
		return impl_actor_address::get_actorserver_map();
	}

	i32_serverid naddress::gatewayid(const nguid& aguid)
	{
		return impl_actor_address::gatewayid(aguid);
	}

	void naddress::add_gatewayid(const nguid& aguid, i32_serverid aserverid)
	{
		impl_actor_address::add_gatewayid(aguid, aserverid);
	}

	void naddress::remove_gatewayid(const nguid& aguid)
	{
		impl_actor_address::remove_gatewayid(aguid);
	}

	void naddress::gatewayid(const std::set<nguid>& aactorset, std::set<i32_serverid>& aserverset)
	{
		impl_actor_address::gatewayid(aactorset, aserverset);
	}
}//namespace ngl