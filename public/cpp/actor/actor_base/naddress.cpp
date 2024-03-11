#include "actor_base.h"
#include "nactortype.h"
#include "naddress.h"
#include "nguid.h"
#include "net.h"

namespace ngl
{
	struct naddress::impl_actor_address
	{
	private:
		naddress::map_guidserver		m_actorserver;
		naddress::map_typeguid			m_actortypeserver;
		naddress::map_servernode		m_session;
		naddress::map_rolegateway		m_rolegateway;
	public:
		inline bool set_node(const np_actornode& anode)
		{
			Try
			{
				int16_t lserverid = anode.m_serverid;
				auto itor = m_session.find(lserverid);
				bool ret = itor == m_session.end();
				actor_node_session& _lnode = ret ? m_session[lserverid] : itor->second;
				_lnode.m_node = anode;
				return ret;
			}
			Catch
			return false;
		}

		inline void print_address()const
		{
			LogLocalError("############################")
			for (const std::pair<nguid, i32_serverid>& ipair : m_actorserver)
			{
				LogLocalError("[%:%][%-%-%]"
					, ipair.first
					, ipair.second
					, nguid::actordataid(ipair.first)
					, nactortype::enum2name((ENUM_ACTOR)nguid::type(ipair.first))
					, nguid::area(ipair.first)
				)
			}
			LogLocalError("############################")
		}

		inline i32_sessionid sessionbyrole(i16_area aarea, i32_actordataid aroleid)
		{
			Try
			{
				i64_actorid lactorrole = nguid::make(ACTOR_ROLE, aarea, aroleid);
				i32_serverid lserverid = get_server(lactorrole);
				Assert(lserverid != -1)
				return get_session(lserverid);
			}Catch;
			return -1;
		}

		inline bool handle(handle_pram& apram)
		{
			if (apram.m_forwardfun != nullptr)
				apram.m_forwardfun(m_session, m_actorserver, apram);
			return true;
		}

		inline void actor_add(i32_serverid aserverid, i64_actorid adataid)
		{
			nguid lguid(adataid);
			m_actorserver[lguid] = aserverid;
			m_actortypeserver[lguid.type()].insert(adataid);
		}

		inline void actor_del(i64_actorid adataid)
		{
			nguid lguid(adataid);
			m_actorserver.erase(lguid);
		}

		// 设置session
		inline void set_session(i32_serverid aserverid, i32_sessionid asession)
		{
			Try
			{
				auto itor = m_session.find(aserverid);
				Assert(itor != m_session.end());
				itor->second.m_session = asession;
				//segpack::add_server(aserverid, asession);
			}Catch;
		}

		// 获取session
		inline i32_sessionid get_session(i32_serverid aserverid)const
		{
			Try
			{
				auto itor = m_session.find(aserverid);
				Assert(itor != m_session.end());
				return itor->second.m_session;
			}Catch;
			return -1;
		}

		// 获取server id
		inline i32_serverid get_server(const nguid& aguid)const
		{
			Try
			{
				auto itor = m_actorserver.find(aguid);
				Assert(itor != m_actorserver.end());
				return itor->second;
			}Catch;
			return -1;
		}

		inline void get_serverlist(ENUM_ACTOR atype, std::set<i32_serverid>& avec)
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
			}Catch;
		}

		inline void foreach(const foreach_callbackfun& afun)
		{
			for (auto&& [_, nsession] : m_session)
			{
				if (afun(nsession) == false)
					return;
			}
		}

		inline void ergodic(const ergodic_callbackfun& afun)
		{
			afun(m_actorserver, m_session);
		}

		inline naddress::map_guidserver& get_actorserver_map()
		{
			return m_actorserver;
		}

		inline i32_serverid get_gatewayid(const nguid& aguid)
		{
			i32_serverid* lpserverid = tools::findmap(m_rolegateway, aguid);
			if (lpserverid == nullptr)
				return -1;
			return *lpserverid;
		}

		inline void add_gatewayid(const nguid& aguid, i32_serverid aserverid)
		{
			m_rolegateway[aguid] = aserverid;
		}

		inline void remove_gatewayid(const nguid& aguid)
		{
			m_rolegateway.erase(aguid);
		}

		inline void get_gatewayid(const std::set<nguid>& aactorset, std::set<i32_serverid>& aserverset)
		{
			for (const nguid& iactorid : aactorset)
			{
				nguid lguid(iactorid);
				i32_serverid* lserverid = tools::findmap(m_rolegateway, lguid);
				if (lserverid == nullptr)
					continue;
				aserverset.insert(*lserverid);
			}
		}
	};

	naddress::naddress()
	{
		m_impl_actor_address.make_unique();
	}

	naddress::~naddress()
	{}

	bool naddress::set_node(const np_actornode& anode)
	{
		return m_impl_actor_address()->set_node(anode);
	}

	void naddress::print_address()
	{
		m_impl_actor_address()->print_address();
	}

	i32_sessionid naddress::sessionbyrole(i16_area aarea, i32_actordataid aroleid)
	{
		return m_impl_actor_address()->sessionbyrole(aarea, aroleid);
	}

	bool naddress::handle(handle_pram& apram)
	{
		return m_impl_actor_address()->handle(apram);
	}

	void naddress::actor_add(i32_serverid aserverid, i64_actorid adataid)
	{
		LogLocalError("######################serverid:actorid [%]:[%]", aserverid, adataid)
		m_impl_actor_address()->actor_add(aserverid, adataid);
	}

	// 添加一组actor
	void naddress::actor_add(i32_serverid aserverid, const std::vector<i64_actorid>& avec)
	{
		for (const i64_actorid item : avec)
			actor_add(aserverid, item);
		//#ifdef _DEBUG
		//		print_address();
		//#endif
	}

	// 删除actor
	void naddress::actor_del(i64_actorid adataid)
	{
		m_impl_actor_address()->actor_del(adataid);
	}

	// 删除一组actor
	void naddress::actor_del(const std::vector<i64_actorid>& avec)
	{
		for (const i64_actorid item : avec)
			actor_del(item);
	}

	// 设置session
	void naddress::set_session(i32_serverid aserverid, i32_sessionid asession)
	{
		m_impl_actor_address()->set_session(aserverid, asession);
	}

	// 获取session
	i32_sessionid naddress::get_session(i32_serverid aserverid)
	{
		return m_impl_actor_address()->get_session(aserverid);
	}

	// 获取server id
	i32_serverid naddress::get_server(const nguid& aguid)
	{
		return m_impl_actor_address()->get_server(aguid);
	}

	void naddress::get_serverlist(ENUM_ACTOR atype, std::set<i32_serverid>& avec)
	{
		m_impl_actor_address()->get_serverlist(atype, avec);
	}

	void naddress::foreach(const foreach_callbackfun& afun)
	{
		m_impl_actor_address()->foreach(afun);
	}

	void naddress::ergodic(const ergodic_callbackfun& afun)
	{
		m_impl_actor_address()->ergodic(afun);
	}

	naddress::map_guidserver& naddress::get_actorserver_map()
	{
		return m_impl_actor_address()->get_actorserver_map();
	}

	i32_serverid naddress::get_gatewayid(const nguid& aguid)
	{
		return m_impl_actor_address()->get_gatewayid(aguid);
	}

	void naddress::add_gatewayid(const nguid& aguid, i32_serverid aserverid)
	{
		m_impl_actor_address()->add_gatewayid(aguid, aserverid);
	}

	void naddress::remove_gatewayid(const nguid& aguid)
	{
		m_impl_actor_address()->remove_gatewayid(aguid);
	}

	void naddress::get_gatewayid(const std::set<nguid>& aactorset, std::set<i32_serverid>& aserverset)
	{
		m_impl_actor_address()->get_gatewayid(aactorset, aserverset);
	}
}//namespace ngl