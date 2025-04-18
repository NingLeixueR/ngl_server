﻿#include "ttab_specialid.h"
#include "actor_events.h"
#include "manage_curl.h"
#include "nsp_server.h"
#include "nsp_client.h"
#include "actor_role.h"
#include "json_write.h"
#include "json_read.h"
#include "nregister.h"
#include "nforward.h"
#include "net.pb.h"
#include "drop.h"
#include "gcmd.h"


namespace ngl
{
	actor_role::actor_role(i16_area aarea, i32_actordataid aroleid, void* adata) :
		actor(
			actorparm
			{
				.m_parm
				{
					.m_type		= ACTOR_ROLE,
					.m_area		= aarea,
					.m_id		= aroleid,
					.m_manage_dbclient = true
				},
				.m_weight = 10,
				.m_timeout = 1000,
				.m_broadcast = true,
			})
		, m_gatewayid(((np_actorswitch_process_role*)(adata))->m_gatewayid)
	{
		assert(aarea == tab_self_area);
	}

	ENUM_ACTOR actor_role::actor_type()
	{
		return ACTOR_ROLE;
	}

	i64_actorid actor_role::actorid(int32_t adata)
	{
		return nguid::make(actor_type(), tab_self_area, adata);
	}

	void actor_role::init()
	{
		// 绑定DB结构:DB.set(this);
		m_info.set(this);
		m_bag.set(this);
		m_task.set(this);
		m_rolekv.set(this);

		// 设置timer_handle定时器
		/*np_timerparm tparm;
		if (make_timerparm::make_interval(tparm, 2) == false)
		{
			log_error()->print("actor_chat::init() make_timerparm::make_interval(tparm, 2) == false!!!");
			return;
		}
		set_timer(tparm);
		*/
	}

	void actor_role::loaddb_finish(bool adbishave)
	{
		log_error()->print("actor_role###loaddb_finish#[{}]", guid());
		sync_data_client();
		m_info.sync_actor_brief();
		loginpay();

		np_eevents_logic_rolelogin lparm;
		lparm.m_type = eevents_logic_rolelogin;
		lparm.m_actorid = id_guid();
		actor_events_logic::trigger_event(eevents_logic_rolelogin, lparm);
		actor_events_logic::trigger_event(eevents_logic_rolelogin, lparm, this);
	}

	void actor_role::nregister()
	{
		// 定时器
		actor::register_timer<actor_role>(&actor_role::timer_handle);

		// 绑定自定义np_消息
		register_handle_custom<actor_role>::func<
			np_actor_disconnect_close
			, mforward<np_gm>
			, np_example_actorid
		>(true);

		// 绑定pb消息
		register_handle_proto<actor_role>::func<
			pbnet::PROBUFF_NET_ROLE_SYNC
			, pbnet::PROBUFF_NET_TASK_RECEIVE_AWARD
		>(true);

		nforward::c2g();
	}

	i32_serverid actor_role::get_getwayserverid()
	{
		return m_gatewayid;
	}

	void actor_role::loginpay()
	{
		// ### 检查是否有充值未发货
		auto lhttp = ngl::manage_curl::make_http();
		ngl::manage_curl::set_mode(lhttp, ngl::ENUM_MODE_HTTP);
		ngl::manage_curl::set_type(lhttp, ngl::ENUM_TYPE_GET);
		ngl::manage_curl::set_url(lhttp, "http://127.0.0.1:800/pay/pay_login.php");

		ngl::manage_curl::set_param(lhttp, std::format("roleid={}", id_guid()).c_str());

		ngl::manage_curl::set_callback(lhttp, [this](int, http_parm& ahttp)
			{
				if (ahttp.m_recvdata.empty())
				{
					return;
				}
				log_error()->print("actor_role::loginpay curl callback [{}]", ahttp.m_recvdata);
				try
				{
					json_read ltempjson(ahttp.m_recvdata.c_str());
					if (ltempjson.check() == false)
					{
						log_error()->print("actor_role::loginpay curl callback fail");
						return;
					}
					std::string lorderid;
					if (ltempjson.read("orderid", lorderid) == false)
					{
						return;
					}
					int32_t lrechargeid = -1;
					if (ltempjson.read("rechargeid", lrechargeid) == false)
					{
						return;
					}						
					int64_t lroleid = -1;
					if (ltempjson.read("roleid", lroleid) == false)
					{
						return;
					}
					
					auto pro = std::make_shared<mforward<np_gm_response>>();
					np_gm_response* lp = pro->add_data();
					prorechange prore;
					ngl::json_write lwrite;
					lwrite.write("operator", "rechange");
					lwrite.write("data", prore);
					lwrite.get(lp->m_json);
					actor::static_send_actor(lroleid, nguid::make(), pro);
				}
				catch (...){}				
			});
		ngl::manage_curl::send(lhttp);
	}

	void actor_role::erase_actor_before()
	{
		np_eevents_logic_roleoffline lparm;
		lparm.m_type = eevents_logic_roleoffline;
		lparm.m_actorid = id_guid();
		actor_events_logic::trigger_event(eevents_logic_roleoffline, lparm);
	}

	void actor_role::handle_after()
	{
		// ### 同步这次消息的背包变动
		m_bag.sync_client();
		if (m_attribute.sync())
		{
			m_attribute.set_sync(false);
			auto pro = std::shared_ptr<pbnet::PROBUFF_NET_SYNC_ATTRIBUTE>();
			m_attribute.topb(*pro);
			send_client(id_guid(), pro);
		}		
	}

	i64_actorid actor_role::roleid()
	{
		return guid();
	}

	void actor_role::update_attribute(EnumModule amodule, attribute_value& avalue)
	{
		m_attribute.updata(amodule, avalue);
	}

	void actor_role::sync_data_client()
	{
		auto pro = std::make_shared<pbnet::PROBUFF_NET_ROLE_SYNC_RESPONSE>();
		*pro->mutable_m_role()	= m_info.get()->getconst();
		*pro->mutable_m_bag()	= m_bag.get()->getconst();
		*pro->mutable_m_task()	= m_task.get()->getconst();
		send_client(id_guid(), pro);
	}

	void actor_role::createorder(std::string& aorder, int32_t arechargeid)
	{
		static std::atomic<int32_t> billnoindex = 0;
		aorder = std::format("{:05d}{:010d}{:010d}{:010d}{:02d}",
			area(), id(), arechargeid, localtime::gettime(), billnoindex.fetch_add(1)
		);
	}

	int32_t actor_role::rechange(std::string& aorderid, int32_t arechargeid, bool agm, bool areporting)
	{
		int32_t lstat = 1;
		int32_t lgold = 0;
		auto psenditem = std::make_shared<np_actor_senditem>();
		tab_recharge* tab = allcsv::tab<tab_recharge>(arechargeid);
		if (tab != nullptr)
		{
			psenditem->m_src = std::format("recharge orderid={} rechargeid={} roleid={}", aorderid, arechargeid, id_guid());
			if (drop::droplist(tab->m_dropid, 1, psenditem->m_item))
			{
				message ltemp(0, nullptr, psenditem.get());
				handle(ltemp);
				lgold += tab->m_gold;
				lgold += tab->m_bonus;
				if (is_first_recharge(arechargeid))
				{
					lgold += tab->m_firstbonus;
				}
				m_info.change_gold(lgold);
			}
			else
			{
				lstat = 2;
			}
		}
		else
		{
			lstat = 3;
		}
		
		if (areporting)
		{
			// ### 发货成功上报gm ###
			auto lhttp = ngl::manage_curl::make_http();
			ngl::manage_curl::set_mode(lhttp, ngl::ENUM_MODE_HTTP);
			ngl::manage_curl::set_type(lhttp, ngl::ENUM_TYPE_GET);
			std::string lurl = std::format("{}/pay/pay_update.php", sysconfig::gmurl());
			ngl::manage_curl::set_url(lhttp, lurl);

			std::string lparm = std::format(
				"orderid={}&gm={}&roleid={}&stat={}",aorderid, (agm ? 1 : 0), id_guid(), lstat
			);

			ngl::manage_curl::set_param(lhttp, lparm.c_str());
			ngl::manage_curl::send(lhttp);
		}

		if (lgold > 0 || !psenditem->m_item.empty())
		{
			auto cpro = std::make_shared<pbnet::PROBUFF_NET_DELIVER_GOODS_RECHARGE>();
			cpro->set_m_rechargeid(arechargeid);
			cpro->set_m_orderid(aorderid);
			cpro->set_m_gold(lgold);
			for (auto itor = psenditem->m_item.begin(); itor != psenditem->m_item.end(); ++itor)
			{
				(*cpro->mutable_m_items())[itor->first] = itor->second;
			}
			send_client(id_guid(), cpro);
		}	

		return lstat;
	}

	bool actor_role::is_first_recharge(int32_t arechargeid)
	{
		const pbdb::db_role& lrole = m_info.get_constrole();
		int lcount = 0;
		for (const auto& item : lrole.m_recharge())
		{
			if (item.m_rechargeid() == arechargeid)
			{
				return false;
			}
		}
		return true;
	}

	bool actor_role::forward_before(const pbnet::PROBUFF_NET_CREATE_FAMIL& adata)
	{
		if (ttab_specialid::m_createfamilconsume > m_info.gold())
		{
			return false;
		}
		m_info.change_gold(-ttab_specialid::m_createfamilconsume);
		return true;
	}

	void actor_role::echo_msg(const char* amsg)
	{
		auto pro = std::make_shared<pbnet::PROBUFF_NET_MSG_RESPONSE>();
		pro->set_m_msg(amsg);
		send_client(id_guid(), pro);
	}

	bool actor_role::timer_handle(const message<np_timerparm>& adata)
	{
		return true;
	}

	bool actor_role::handle(const message<np_arg_null>&)
	{
		return true;
	}
}//namespace ngl