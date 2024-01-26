#include "actor_address.h"
#include "net.h"
#include "actor_enum.h"
#include "actor_guid.h"
#include "actor_base.h"

namespace ngl
{
	struct actor_address::impl_actor_address
	{
	private:
		actor_address::map_guidserver		m_actorserver;
		actor_address::map_typeguid			m_actortypeserver;
		actor_address::map_servernode		m_session;
		actor_address::map_rolegateway		m_rolegateway;
	public:
		inline bool set_node(const actor_node& anode)
		{
			Try
			{
				const std::vector<i16_actortype>&lvec = anode.m_actortype;
				int16_t lserverid = anode.m_serverid;
				auto itor = m_session.find(lserverid);
				bool ret = itor == m_session.end();
				actor_node_session& _lnode = ret ? m_session[lserverid] : itor->second;
				_lnode.m_node = anode;
				return ret;
			}
			Catch;
			return false;
		}

		inline void print_address()
		{
			LogLocalError("############################");
			for (auto&& [_key, _value] : m_actorserver)
			{
				LogLocalError("[%:%][%-%-%]", _key, _value, actor_guid::actordataid(_key), actor_typename::enum2name((ENUM_ACTOR)actor_guid::type(_key)), actor_guid::area(_key));
			}
			LogLocalError("############################");
		}

		inline i32_sessionid sessionbyrole(i16_area aarea, i32_actordataid aroleid)
		{
			Try
			{
				i64_actorid lactorrole = actor_guid::make(ACTOR_ROLE, aarea, aroleid);
				i32_serverid lserverid = get_server(lactorrole);
				Assert(lserverid != -1);
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
			actor_guid lguid(adataid);
			m_actorserver[lguid] = aserverid;
			m_actortypeserver[lguid.type()].insert(adataid);
		}

		inline void actor_del(i64_actorid adataid)
		{
			actor_guid lguid(adataid);
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
		inline i32_sessionid get_session(i32_serverid aserverid)
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
		inline i32_serverid get_server(const actor_guid& aguid)
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

		inline actor_address::map_guidserver& get_actorserver_map()
		{
			return m_actorserver;
		}

		inline i32_serverid get_gatewayid(const actor_guid& aguid)
		{
			i32_serverid* lpserverid = tools::findmap(m_rolegateway, aguid);
			if (lpserverid == nullptr)
				return -1;
			return *lpserverid;
		}

		inline void add_gatewayid(const actor_guid& aguid, i32_serverid aserverid)
		{
			m_rolegateway[aguid] = aserverid;
		}

		inline void remove_gatewayid(const actor_guid& aguid)
		{
			m_rolegateway.erase(aguid);
		}

		inline void get_gatewayid(const std::set<actor_guid>& aactorset, std::set<i32_serverid>& aserverset)
		{
			for (const actor_guid& iactorid : aactorset)
			{
				actor_guid lguid(iactorid);
				i32_serverid* lserverid = tools::findmap(m_rolegateway, lguid);
				if (lserverid == nullptr)
					continue;
				aserverset.insert(*lserverid);
			}
		}
	};

	actor_address::actor_address()
	{
		m_impl_actor_address.make_unique();
	}

	actor_address::~actor_address()
	{}

	bool actor_address::set_node(const actor_node& anode)
	{
		return m_impl_actor_address()->set_node(anode);
	}

	void actor_address::print_address()
	{
		m_impl_actor_address()->print_address();
	}

	i32_sessionid actor_address::sessionbyrole(i16_area aarea, i32_actordataid aroleid)
	{
		return m_impl_actor_address()->sessionbyrole(aarea, aroleid);
	}

	bool actor_address::handle(handle_pram& apram)
	{
		return m_impl_actor_address()->handle(apram);
	}

	void actor_address::actor_add(i32_serverid aserverid, i64_actorid adataid)
	{
		LogLocalError("######################serverid:actorid [%]:[%]", aserverid, adataid);
		m_impl_actor_address()->actor_add(aserverid, adataid);
	}

	// 添加一组actor
	void actor_address::actor_add(i32_serverid aserverid, const std::vector<i64_actorid>& avec)
	{
		for (const i64_actorid item : avec)
			actor_add(aserverid, item);
		//#ifdef _DEBUG
		//		print_address();
		//#endif
	}

	// 删除actor
	void actor_address::actor_del(i64_actorid adataid)
	{
		m_impl_actor_address()->actor_del(adataid);
	}

	// 删除一组actor
	void actor_address::actor_del(const std::vector<i64_actorid>& avec)
	{
		for (const i64_actorid item : avec)
			actor_del(item);
	}

	// 设置session
	void actor_address::set_session(i32_serverid aserverid, i32_sessionid asession)
	{
		m_impl_actor_address()->set_session(aserverid, asession);
	}

	// 获取session
	i32_sessionid actor_address::get_session(i32_serverid aserverid)
	{
		return m_impl_actor_address()->get_session(aserverid);
	}

	// 获取server id
	i32_serverid actor_address::get_server(const actor_guid& aguid)
	{
		return m_impl_actor_address()->get_server(aguid);
	}

	void actor_address::get_serverlist(ENUM_ACTOR atype, std::set<i32_serverid>& avec)
	{
		m_impl_actor_address()->get_serverlist(atype, avec);
	}

	void actor_address::foreach(const foreach_callbackfun& afun)
	{
		m_impl_actor_address()->foreach(afun);
	}

	void actor_address::ergodic(const ergodic_callbackfun& afun)
	{
		m_impl_actor_address()->ergodic(afun);
	}

	actor_address::map_guidserver& actor_address::get_actorserver_map()
	{
		return m_impl_actor_address()->get_actorserver_map();
	}

	i32_serverid actor_address::get_gatewayid(const actor_guid& aguid)
	{
		return m_impl_actor_address()->get_gatewayid(aguid);
	}

	void actor_address::add_gatewayid(const actor_guid& aguid, i32_serverid aserverid)
	{
		m_impl_actor_address()->add_gatewayid(aguid, aserverid);
	}

	void actor_address::remove_gatewayid(const actor_guid& aguid)
	{
		m_impl_actor_address()->remove_gatewayid(aguid);
	}

	void actor_address::get_gatewayid(const std::set<actor_guid>& aactorset, std::set<i32_serverid>& aserverset)
	{
		m_impl_actor_address()->get_gatewayid(aactorset, aserverset);
	}
}
