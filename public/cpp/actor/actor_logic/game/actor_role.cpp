#include "actor_role.h"
#include "actor_register.h"
#include "gameclient_forward.h"
#include "net.pb.h"
#include "actor_roleitem.h"
#include "drop.h"
#include "manage_curl.h"
#include "ojson.h"

namespace ngl
{
	actor_role::actor_role(i16_area aarea, i32_actordataid aroleid, void* adata) :
		actor(
			actorparm
			{
				.m_parm
				{
					.m_type = ACTOR_ROLE,
					.m_area = aarea,
					.m_id = aroleid,
					.m_manage_dbclient = true
				},
				.m_weight = 0x7fffffff,
				.m_broadcast = true,
			})
		, m_gatewayid(((actor_switch_process_role*)(adata))->m_gatewayid)
		, m_playactorid(0)
	{
		assert(aarea == ttab_servers::tab()->m_area);
	}

	i32_serverid actor_role::get_getwayserverid()
	{
		return m_gatewayid;
	}

	void actor_role::init()
	{
		m_info.set(this);
		m_bag.set(this);
		m_task.set(this);
	}

	actor_role::~actor_role() 
	{}

	void actor_role::actor_register()
	{
		// 定时器
		register_timer<actor_role>(&actor_role::timer_handle);

		register_actor<EPROTOCOL_TYPE_CUSTOM, actor_role>(
			true
			, dregister_fun_handle(actor_role, actor_disconnect_close)
		);

		register_actor<EPROTOCOL_TYPE_PROTOCOLBUFF, actor_role>(
			true
			, dregister_fun_handle(actor_role, pbnet::PROBUFF_NET_ROLE_SYNC)
			, dregister_fun_handle(actor_role, pbnet::PROBUFF_NET_MATCHING_SUCCESS_RESPONSE)
			, dregister_fun_handle(actor_role, mforward<GM::PROBUFF_GM_RECHARGE>)
		);

		// 协议注册
		gameclient_forward::c2g();
	}

	void actor_role::loginpay()
	{
		// ### 检查是否有充值未发货
		ngl::_http* lhttp = ngl::manage_curl::make_http();
		ngl::manage_curl::set_mode(*lhttp, ngl::ENUM_MODE_HTTP);
		ngl::manage_curl::set_type(*lhttp, ngl::ENUM_TYPE_GET);
		ngl::manage_curl::set_url(*lhttp, "http://127.0.0.1:800/pay/pay_login.php");

		std::stringstream lstream;
		lstream << "roleid=" << id_guid();

		ngl::manage_curl::set_param(*lhttp, lstream.str());
		ngl::manage_curl::set_callback(*lhttp, [](int, _http& ahttp)
			{
				if (ahttp.m_recvdata.empty())
					return;
				try
				{
					ojson ltempjson(ahttp.m_recvdata.c_str());
					if (ltempjson.check() == false)
					{
						LogLocalError("ngl::manage_curl::callback fail [%]", ahttp.m_recvdata);
						return;
					}
					std::pair<const char*, const char*> orderid("orderid", "");
					if (ltempjson >> orderid == false)
						return;
					std::pair<const char*, const char*> rechargeid("rechargeid", 0);
					if (ltempjson >> rechargeid == false)
						return;
					std::pair<const char*, const char*> roleid("roleid", 0);
					if (ltempjson >> roleid == false)
						return;
					auto prot = std::make_shared<GM::PROBUFF_GM_RECHARGE>();
					auto pro = std::make_shared<mforward<GM::PROBUFF_GM_RECHARGE>>(-1, prot);
					pro->data()->set_m_orderid(orderid.second);
					pro->data()->set_m_rechargeid(boost::lexical_cast<int32_t>(rechargeid.second));
					pro->data()->set_m_roleid(boost::lexical_cast<int64_t>(roleid.second));
					actor::static_send_actor(boost::lexical_cast<int64_t>(roleid.second), actor_guid::make(), pro);
				}
				catch (...)
				{

				}				
			});
		ngl::manage_curl::getInstance().send(lhttp);
	}

	void actor_role::loaddb_finish(bool adbishave)
	{
		LogLocalError("actor_role###loaddb_finish#[%]", actor_guid(id_guid()));
		sync_data_client();
		m_info.sync_actor_roleinfo();
		loginpay();
	}

	void actor_role::handle_after()
	{
		// ### 同步这次消息的背包变动
		m_bag.sync_client();
		if (m_attribute.issync())
		{
			m_attribute.set_issync(false);
			auto pro = std::shared_ptr<pbnet::PROBUFF_NET_SYNC_ATTRIBUTE>();
			m_attribute.topb(*pro);
			send2client(pro);
		}		
	}

	i64_actorid actor_role::roleid()
	{
		return m_info.get()->getconst().m_id();
	}

	void actor_role::sync_data_client()
	{
		auto pro = std::make_shared<pbnet::PROBUFF_NET_ROLE_SYNC_RESPONSE>();
		*pro->mutable_m_role() = m_info.get()->getconst();
		*pro->mutable_m_bag() = m_bag.get()->getconst();
		*pro->mutable_m_task() = m_task.get()->getconst();
		send2client(pro);
		LogLocalError("[sync]###[%]", m_info.get()->getconst().m_base().m_name());
	}

	void actor_role::createorder(std::string& aorder, int32_t arechargeid)
	{
		static int billnoindex = 0;
		char lbillno[128];
		sprintf(lbillno,
			"%05d%010d%010d%010d%02d",
			area(), id(), arechargeid, localtime::gettime(), ++billnoindex);
		aorder = lbillno;
	}

	bool actor_role::handle(message<pbnet::PROBUFF_NET_RECHARGE>& adata)
	{
		auto pro = std::make_shared<pbnet::PROBUFF_NET_RECHARGE_RESPONSE>();
		pro->set_m_rechargeid(adata.m_data->m_rechargeid());
		const pbdb::db_role& lrole = m_info.m_role();
		tab_recharge* tab = allcsv::tab<tab_recharge>(adata.m_data->m_rechargeid());
		if (tab == nullptr)
		{
			LogLocalError("tab_recharge id[%] not find", adata.m_data->m_rechargeid());
			pro->set_m_stat(pbnet::PROBUFF_NET_RECHARGE_RESPONSE::Estat_NotRechargeId);
			send2client(pro);
			return true;
		}
		if (tab->m_count > 0)
		{
			int lcount = 0;
			for (auto& item : lrole.m_recharge())
			{
				if (item.m_rechargeid() == adata.m_data->m_rechargeid())
					++lcount;
			}
			if (lcount > tab->m_count)
			{
				pro->set_m_stat(pbnet::PROBUFF_NET_RECHARGE_RESPONSE::Estat_MaxCount);
				send2client(pro);
				return true;
			}
		}
		createorder(*pro->mutable_m_orderid(), adata.m_data->m_rechargeid());
		pro->set_m_stat(pbnet::PROBUFF_NET_RECHARGE_RESPONSE::Estat_Success);
		send2client(pro);
		return true;
	}

	bool actor_role::is_first_recharge(int32_t arechargeid)
	{
		const pbdb::db_role& lrole = m_info.m_role();
		int lcount = 0;
		for (auto& item : lrole.m_recharge())
		{
			if (item.m_rechargeid() == arechargeid)
				return false;
		}
		return true;
	}

	bool actor_role::handle(message<mforward<GM::PROBUFF_GM_RECHARGE>>& adata)
	{
		// ### 
		auto prot = std::make_shared<GM::PROBUFF_GM_RECHARGE_RESPONSE>();
		auto pro = std::make_shared<mforward<GM::PROBUFF_GM_RECHARGE_RESPONSE>>(adata.m_data->identifier(), prot);
		int32_t lrechargeid = adata.m_data->data()->m_rechargeid();
		pro->data()->set_m_rechargeid(lrechargeid);
		tab_recharge* tab = allcsv::tab<tab_recharge>(lrechargeid);
		if (tab == nullptr)
		{
			LogLocalError("tab_recharge id[%] not find", lrechargeid);
			pro->data()->set_m_stat(1);
			return true;
		}
		int lgold = 0;
		lgold += tab->m_gold;
		lgold += tab->m_bonus;
		if (is_first_recharge(lrechargeid))
		{
			lgold += tab->m_firstbonus;
		}

		m_info.change_gold(lgold);


		std::map<int, int> ldropmap;
		if (tab->m_dropid > 0)
		{
			drop::droplist(tab->m_dropid, 1, ldropmap);
			m_bag.add_item(ldropmap);
			//drop::use(this, tab->m_dropid, 1);
		}
		if (adata.m_data->identifier() > 0)
		{
			send_actor(actor_guid::make_self(ACTOR_GM), pro);
		}

		auto cpro = std::make_shared<pbnet::PROBUFF_NET_DELIVER_GOODS_RECHARGE>();
		cpro->set_m_rechargeid(lrechargeid);
		cpro->set_m_orderid(adata.m_data->data()->m_orderid());
		cpro->set_m_gold(lgold);
		for(auto itor = ldropmap.begin(); itor != ldropmap.end();++itor)
		{
			(*cpro->mutable_m_items())[itor->first] = itor->second;
		}
		send2client(cpro);

		// ### 发货成功上报gm ###
		ngl::_http* lhttp = ngl::manage_curl::make_http();
		ngl::manage_curl::set_mode(*lhttp, ngl::ENUM_MODE_HTTP);
		ngl::manage_curl::set_type(*lhttp, ngl::ENUM_TYPE_GET);
		ngl::manage_curl::set_url(*lhttp, "http://127.0.0.1:800/pay/pay_update.php");

		std::stringstream lstream;
		lstream
			<< "orderid=" << adata.m_data->data()->m_orderid()
			<< "&gm=0"
			<< "&roleid=" << id_guid()
			<< "&stat=0";

		ngl::manage_curl::set_param(*lhttp, lstream.str());
		ngl::manage_curl::getInstance().send(lhttp);
		
		return true;
	}

	bool actor_role::timer_handle(message<timerparm>& adata)
	{
		return true;
	}
}