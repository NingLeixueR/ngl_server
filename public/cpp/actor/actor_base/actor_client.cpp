#include "server_session.h"
#include "actor_client.h"
#include "actor_server.h"
#include "actor_manage.h"
#include "nregister.h"
#include "naddress.h"
#include "net.h"

namespace ngl
{
	struct actor_client::impl_actor_client
	{
		impl_actor_client(const impl_actor_client&) = delete;
		impl_actor_client& operator=(const impl_actor_client&) = delete;

		std::map<i32_serverid, std::list<std::function<void()>>>	m_connectfun;
		std::set<uint32_t>											m_connectserverid;

		impl_actor_client()
		{}
	};

	actor_client::actor_client() :
		actor(
			actorparm
			{
				.m_parm
				{
					.m_type = ACTOR_CLIENT,
					.m_area = tab_self_area,
					.m_id	= nconfig::m_nodeid
				},
				.m_weight	= 0x7fffffff
			})
	{
		m_impl_actor_client.make_unique();
	}

	actor_client::~actor_client()
	{
	}

	i64_actorid actor_client::actorid()
	{
		return nguid::make(ACTOR_CLIENT, tab_self_area, nconfig::m_nodeid);
	}

	void actor_client::nregister()
	{
		//# 设置未找到协议处理函数
		nrfun<actor_client, EPROTOCOL_TYPE_CUSTOM>::instance().set_notfindfun(
			[](int, handle_pram& apram) 
			{
				naddress::forward(apram);
			});
		nrfun<actor_client, EPROTOCOL_TYPE_PROTOCOLBUFF>::instance().set_notfindfun(
			[](int, handle_pram& apram)
			{
				naddress::forward(apram);
			});

		//# 注册协议
		register_handle_custom<actor_client>::func<
			np_actornode_register_response
			, np_actorclient_node_connect
			, np_actornode_connect_task
			, np_actor_gatewayid_updata
			, np_actornode_update_mass
			, np_actor_server_register
			, np_connect_actor_server
			, np_actornode_update
		>(true);
	}

	void set_node(int32_t aserverid, int asession)
	{
		tab_servers const* tab = ttab_servers::instance().tab(aserverid);
		if (tab == nullptr)
		{
			return;
		}
		nactornode lnode;
		lnode.m_name = tab->m_name;
		lnode.m_serverid = aserverid;
		lnode.m_nodetype = tab->m_type;
		naddress::set_node(lnode);
		naddress::set_session(aserverid, asession);
	}

	bool actor_client::handle(const message<np_connect_actor_server>& adata)
	{
		const tab_servers* tab = ttab_servers::instance().tab();
		const tab_servers* tabactor = ttab_servers::instance().tab(adata.get_data()->m_serverid);
		assert(tab != nullptr && tabactor != nullptr);

		i64_actorid	lactorid = id_guid();
		i64_actorid lactorserve = actor_server::actorid();
		set_node(tabactor->m_id, adata.get_data()->m_session);
		naddress::add_actor_address(tabactor->m_id, lactorserve);

		//注册结点
		np_actornode_register lpram
		{
			.m_node
			{
				.m_name = std::format(
					"node<id:{},type:{},name:{},tcount:{},area:{}>",
					tab->m_id, em<NODE_TYPE>::get_name(tab->m_type), tab->m_name, tab->m_tcount, tab->m_area
				),
				.m_nodetype = tab->m_type,
				.m_serverid = tab->m_id,
			}
		};
		actor_manage::instance().get_type(lpram.m_node.m_actortype);
		naddress::ergodic(
			[&lpram](const std::map<nguid, i32_serverid>& aactorserver, const std::map<i32_serverid, actor_node_session>&)
			{
				for (const std::pair<const nguid, i32_serverid>& item : aactorserver)
				{
					if (lpram.m_node.m_serverid == item.second)
					{
						lpram.m_add.push_back(item.first);
					}
				}
				return true;
			});
		nets::sendbysession(adata.get_data()->m_session, lpram, lactorserve, lactorid);
		return true;
	}

	void actor_client::actor_server_register(i32_serverid aactorserver)
	{
		if (nconfig::m_nodetype == NODE_TYPE::ROBOT)
			return;
		nets::connect(aactorserver, [this, aactorserver](int asession)
			{
				auto pro = std::make_shared<np_connect_actor_server>();
				pro->m_serverid = aactorserver;
				pro->m_session = asession;
				actor::send_actor(id_guid(), pro);
			}, false, true);
	}

	void actor_client::actor_server_register()
	{
		send_actor(id_guid(), std::make_shared<np_actor_server_register>());
	}

	bool actor_client::handle(const message<np_actor_server_register>& adata)
	{
		if (nconfig::m_nodetype == NODE_TYPE::ROBOT)
		{
			return true;
		}
		// # 需要尝试连接ActorServer结点 并向其注册自己
		tools::no_core_dump(ttab_servers::instance().node_type() != ngl::ACTORSERVER);
		tools::no_core_dump(ttab_servers::instance().node_type() != ngl::ROBOT);
		const tab_servers* tab = ttab_servers::instance().tab();
		for (int32_t id : tab->m_actorserver)
		{
			actor_server_register(id);
		}
		return true;
	}

	bool isactiv_connect(i32_serverid aserverid)
	{
		return nconfig::m_nodeid > aserverid;
	}

	void actor_client::activ_connect(i32_serverid aserverid)
	{
		if (isactiv_connect(aserverid))
		{
			nets::connect(aserverid, [this, aserverid](i32_session asession)
				{
					log_error()->print("actor_client::activ_connect connect node:{} success", aserverid);

					set_node(aserverid, asession);

					np_actorclient_node_connect pro;
					pro.m_id = nconfig::m_nodeid;
					nets::sendbysession(asession, pro, nguid::moreactor(), id_guid());
				}, false, true);
		}
	}

	bool actor_client::handle(const message<np_actornode_register_response>& adata)
	{
		if (nconfig::m_nodetype == NODE_TYPE::ROBOT)
		{
			return true;
		}
		auto lparm = adata.get_data();
		const tab_servers* tab = ttab_servers::instance().tab();
		tools::no_core_dump(tab != nullptr);
		for (const nactornode& node : lparm->m_vec)
		{
			if (server_session::sessionid(node.m_serverid) == -1)
			{
				// # 比较id(较大的主动连接较小的)
				// # NODE_TYPE lservertype = ttab_servers::node_type(node.m_serverid);
				activ_connect(node.m_serverid);
			}
		}
		return true;
	}

	void node_update(actor_client* aclient, i32_serverid alocalserverid, i32_session asession)
	{
		np_actornode_update lpro;
		lpro.m_id = alocalserverid;
		for (const std::pair<const nguid, i32_serverid>& item : naddress::get_actorserver_map())
		{
			if (alocalserverid == item.second)
			{
				lpro.m_add.push_back(item.first);
			}
		}
		nets::sendbysession(asession, lpro, nguid::moreactor(), aclient->id_guid());
	}

	bool actor_client::handle(const message<np_actorclient_node_connect>& adata)
	{
		if (nconfig::m_nodetype == NODE_TYPE::ROBOT)
		{
			return true;
		}
		auto lparm = adata.get_data();
		auto lpack = adata.get_pack();
		i32_serverid lserverid = lparm->m_id;
		tools::no_core_dump(lserverid != nconfig::m_nodeid);

		node_update(this, nconfig::m_nodeid, lpack->m_id);

		server_session::add(lserverid, lpack->m_id);

		set_node(lserverid, lpack->m_id);
		naddress::set_session(lserverid, lpack->m_id);

		// 主动连接
		if (isactiv_connect(lserverid) == false)
		{
			np_actorclient_node_connect pro;
			pro.m_id = nconfig::m_nodeid;
			nets::sendbysession(lpack->m_id, pro, nguid::moreactor(), id_guid());
		}

		set_connect_fnish(lparm->m_id);
		connect_fnish();

		// 当前结点类型如果是登陆服务器，且连接的结点为[GAME/GATEWAY]
		NODE_TYPE lservertype = ttab_servers::instance().node_type(lserverid);
		if (xmlnode::node_type() == ngl::LOGIN && (lservertype == ngl::GAME || lservertype == ngl::GATEWAY))
		{
			auto pro = std::make_shared<np_actorserver_connect>();
			pro->m_serverid = lserverid;
			nguid lguid = nguid::make_self(ACTOR_LOGIN);
			handle_pram lparm = handle_pram::create(lguid, guid(), pro);
			actor_manage::instance().push_task_id(lguid, lparm, false);
		}
		return true;
	}

	bool actor_client::handle(const message<np_actornode_update>& adata)
	{
		Try
		{
			auto lparm = adata.get_data();
			naddress::add_actor_address(lparm->m_id, lparm->m_add);
			naddress::del_actor_address(lparm->m_del);
		}Catch
		return true;
	}
	
	bool actor_client::handle(const message<np_actornode_update_mass>& adata)
	{
		auto lparm = adata.get_data();
		auto lpack = adata.get_pack();
		int32_t lthreadid = adata.thread();

		message<np_actornode_update> lmessage(lthreadid, lpack, (np_actornode_update*)&lparm->m_mass);

		handle(lmessage);

		if (nconfig::m_nodetype != NODE_TYPE::ROBOT)
		{
			auto pro = std::make_shared<np_actornode_update_server>();
			pro->m_data = lparm->m_mass;
			send_actor(actor_server::actorid(), pro);
		}
		
		if (lparm->m_fun != nullptr)
		{
			lparm->m_fun();
		}
		return true;
	}

	void actor_client::set_connect_fnish(i32_serverid aserverid)
	{
		m_impl_actor_client()->m_connectserverid.insert(aserverid);
	}

	void actor_client::connect_fnish()
	{
		auto& lconnectfun = m_impl_actor_client()->m_connectfun;
		const std::set<uint32_t>& lconnectserverid = m_impl_actor_client()->m_connectserverid;
		if (lconnectfun.empty())
		{
			return;
		}
		for (auto itor = lconnectfun.begin(); itor != lconnectfun.end(); ++itor)
		{
			if (lconnectserverid.contains(itor->first) == false)
			{
				continue;
			}
			for (const auto& fun : itor->second)
			{
				fun();
			}
			itor = lconnectfun.erase(itor);
			if (itor == lconnectfun.end())
			{
				return;
			}
		}
	}
	
	bool actor_client::handle(const message<np_actornode_connect_task>& adata)
	{
		if (nconfig::m_nodetype == NODE_TYPE::ROBOT)
		{
			return true;
		}
		Try
		{
			auto lparm = adata.get_data();
			const std::set<uint32_t>& lconnectserverid = m_impl_actor_client()->m_connectserverid;
			if(lconnectserverid.contains(lparm->m_serverid))
			{
				lparm->m_fun();
				return true;
			}
			m_impl_actor_client()->m_connectfun[lparm->m_serverid].push_back(lparm->m_fun);
		}Catch
		return true;
	}
	
	bool actor_client::handle(const message<np_actor_gatewayid_updata>& adata)
	{
		if (nconfig::m_nodetype == NODE_TYPE::ROBOT)
		{
			return true;
		}
		const auto lparm = adata.get_data();
		if (lparm->m_isremove)
		{
			naddress::remove_gatewayid(lparm->m_actorid);
		}
		else
		{
			naddress::add_gatewayid(lparm->m_actorid, lparm->m_gatewayid);
		}
		return true;
	}
}//namespace ngl