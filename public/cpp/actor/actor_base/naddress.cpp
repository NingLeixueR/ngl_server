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
			Try
			{
				auto itor = m_session.find(anode.m_serverid);
				if (itor != m_session.end())
				{
					itor->second.m_node = anode;
					return false;
				}
				else
				{
					m_session.try_emplace(anode.m_serverid, anode);
					return true;
				}
			}
			Catch
			return false;
		}

		static void print_address()
		{
			auto lstream = log_error();
			(*lstream) << "+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+" << std::endl;
			for (const std::pair<const nguid, i32_serverid>& ipair : m_actorserver)
			{
				(*lstream) << std::format(
					"[{}:{}][{}-{}-{}]", 
					ipair.first,
					ipair.second,
					nguid::actordataid(ipair.first),
					em<ENUM_ACTOR>::get_name((ENUM_ACTOR)nguid::type(ipair.first)),
					nguid::area(ipair.first)
				) << std::endl;
			}
			(*lstream) << "+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+" << std::endl;
			(*lstream).print("");
		}

		static i32_sessionid sessionbyrole(i16_area aarea, i32_actordataid aroleid)
		{
			Try
			{
				i64_actorid lactorrole = nguid::make(ACTOR_ROLE, aarea, aroleid);
				i32_serverid lserverid = get_server(lactorrole);
				Assert(lserverid != -1)
				return get_session(lserverid);
			}Catch
			return -1;
		}

		static bool forward(handle_pram& apram)
		{
			if (apram.m_forwardfun != nullptr)
				apram.m_forwardfun(m_session, m_actorserver, apram);
			return true;
		}

		static void actor_add(i32_serverid aserverid, i64_actorid adataid)
		{
			nguid lguid(adataid);
			m_actorserver[lguid] = aserverid;
			m_actortypeserver[lguid.type()].insert(adataid);
		}

		static void actor_del(i64_actorid adataid)
		{
			nguid lguid(adataid);
			m_actorserver.erase(lguid);
		}

		static void set_session(i32_serverid aserverid, i32_sessionid asession)
		{
			Try
			{
				auto itor = m_session.find(aserverid);
				Assert(itor != m_session.end())
				itor->second.m_session = asession;
			}Catch
		}

		static i32_sessionid get_session(i32_serverid aserverid)
		{
			Try
			{
				auto itor = m_session.find(aserverid);
				Assert(itor != m_session.end())
				return itor->second.m_session;
			}Catch
			return -1;
		}

		static i32_serverid get_server(const nguid& aguid)
		{
			Try
			{
				auto itor = m_actorserver.find(aguid);
				Assert(itor != m_actorserver.end())
				return itor->second;
			}Catch
			return -1;
		}

		static void get_serverlist(ENUM_ACTOR atype, std::set<i32_serverid>& avec)
		{
			Try
			{
				auto itor = m_actortypeserver.find(atype);
				if (itor != m_actortypeserver.end())
				{
					for (i64_actorid lid : itor->second)
					{
						avec.insert(get_server(lid));
					}
				}
			}Catch
		}

		static void foreach(const naddress::foreach_callbackfun& afun)
		{
			for (const auto& [_, nsession] : m_session)
			{
				if (afun(nsession) == false)
					return;
			}
		}

		static void ergodic(const naddress::ergodic_callbackfun& afun)
		{
			afun(m_actorserver, m_session);
		}

		static naddress::map_guidserver& get_actorserver_map()
		{
			return m_actorserver;
		}

		static i32_serverid get_gatewayid(const nguid& aguid)
		{
			const i32_serverid* lpserverid = tools::findmap(m_rolegateway, aguid);
			if (lpserverid == nullptr)
				return -1;
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

		static void get_gatewayid(
			const std::set<nguid>& aactorset, 
			std::set<i32_serverid>& aserverset
		)
		{
			for (const nguid& iactorid : aactorset)
			{
				nguid lguid(iactorid);
				const i32_serverid* lserverid = tools::findmap(m_rolegateway, lguid);
				if (lserverid == nullptr)
					continue;
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

	void naddress::actor_add(i32_serverid aserverid, i64_actorid adataid)
	{
		impl_actor_address::actor_add(aserverid, adataid);
	}

	void naddress::actor_add(i32_serverid aserverid, const std::vector<i64_actorid>& avec)
	{
		for (const i64_actorid item : avec)
			actor_add(aserverid, item);
		//#ifdef _DEBUG
		//		print_address();
		//#endif
	}

	void naddress::actor_del(i64_actorid adataid)
	{
		impl_actor_address::actor_del(adataid);
	}

	void naddress::actor_del(const std::vector<i64_actorid>& avec)
	{
		for (const i64_actorid item : avec)
			actor_del(item);
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

	i32_serverid naddress::get_gatewayid(const nguid& aguid)
	{
		return impl_actor_address::get_gatewayid(aguid);
	}

	void naddress::add_gatewayid(const nguid& aguid, i32_serverid aserverid)
	{
		impl_actor_address::add_gatewayid(aguid, aserverid);
	}

	void naddress::remove_gatewayid(const nguid& aguid)
	{
		impl_actor_address::remove_gatewayid(aguid);
	}

	void naddress::get_gatewayid(
		const std::set<nguid>& aactorset, 
		std::set<i32_serverid>& aserverset
	)
	{
		impl_actor_address::get_gatewayid(aactorset, aserverset);
	}
}//namespace ngl