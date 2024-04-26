#include "manage_curl.h"
#include "nroleitems.h"
#include "actor_role.h"
#include "nregister.h"
#include "nforward.h"
#include "net.pb.h"
#include "ojson.h"
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
					.m_type = ACTOR_ROLE,
					.m_area = aarea,
					.m_id = aroleid,
					.m_manage_dbclient = true
				},
				.m_weight = 0x7fffffff,
				.m_broadcast = true,
			})
		, m_gatewayid(((np_actorswitch_process_role*)(adata))->m_gatewayid)
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

	void actor_role::nregister()
	{
		// 定时器
		register_timer<actor_role>(&actor_role::timer_handle);

		register_actor<EPROTOCOL_TYPE_CUSTOM, actor_role>(
			true
			, dregister_fun_handle(actor_role, np_actor_disconnect_close)
			, dregister_fun_handle(actor_role, mforward<np_gm>)
		);

		register_actor<EPROTOCOL_TYPE_PROTOCOLBUFF, actor_role>(
			true
			, dregister_fun_handle(actor_role, pbnet::PROBUFF_NET_ROLE_SYNC)
			, dregister_fun_handle(actor_role, pbnet::PROBUFF_NET_MATCHING_SUCCESS_RESPONSE)
			, dregister_fun_handle(actor_role, pbnet::PROBUFF_NET_TASK_RECEIVE_AWARD)
		);

		// 协议注册
		nforward::c2g();
	}

	struct prorechange
	{
		std::string m_orderid;
		int32_t m_rechargeid;
		int64_t m_roleid;

		jsonfunc("orderid", m_orderid, "rechargeid", m_rechargeid, "roleid", m_roleid)
	};

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
					std::string lorderid;
					if (ltempjson.read("orderid", lorderid) == false)
						return;
					int32_t lrechargeid = -1;
					if (ltempjson.read("rechargeid", lrechargeid) == false)
						return;
					int64_t lroleid = -1;
					if (ltempjson.read("roleid", lroleid) == false)
						return;
					
					
					auto pro = std::make_shared<mforward<np_gm_response>>();
					np_gm_response* lp = pro->add_data();
					prorechange prore;
					ngl::ijson lwrite;
					lwrite.write("operator", "rechange");
					lwrite.write("data", prore);
					lwrite.get(lp->m_json);
					actor::static_send_actor(lroleid, nguid::make(), pro);
				}
				catch (...)
				{

				}				
			});
		ngl::manage_curl::getInstance().send(lhttp);
	}

	void actor_role::loaddb_finish(bool adbishave)
	{
		LogLocalError("actor_role###loaddb_finish#[%]", nguid(id_guid()));
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
				message<np_actor_senditem> ltemp(0, nullptr, psenditem.get());
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
			ngl::_http* lhttp = ngl::manage_curl::make_http();
			ngl::manage_curl::set_mode(*lhttp, ngl::ENUM_MODE_HTTP);
			ngl::manage_curl::set_type(*lhttp, ngl::ENUM_TYPE_GET);
			ngl::manage_curl::set_url(*lhttp, "http://127.0.0.1:800/pay/pay_update.php");

			std::stringstream lstream;
			lstream
				<< "orderid=" << aorderid
				<< "&gm=" << (agm ? 1 : 0)
				<< "&roleid=" << id_guid()
				<< "&stat=" << lstat;

			ngl::manage_curl::set_param(*lhttp, lstream.str());
			ngl::manage_curl::getInstance().send(lhttp);
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
			send2client(cpro);
		}	

		return lstat;
	}

	bool actor_role::handle(message<mforward<np_gm>>& adata)
	{
		ngl::ojson lojson(adata.m_data->data()->m_json.c_str());
		std::string loperator;
		if (lojson.read("operator", loperator) == false)
		{
			return true;
		}

		static std::map<std::string, std::function<void(int, ngl::ojson&)>> lcmd;
		if (lcmd.empty())
		{
			lcmd["pay"] = [this](int id, ngl::ojson& aos)
				{
					struct pay
					{
						std::string m_orderid;
						int32_t m_rechargeid;
						jsonfunc("orderid", m_orderid, "rechargeid", m_rechargeid)
					};
					pay lpay;
					if (aos.read("data", lpay) == false)
						return;

					// 返回 {"data":int32_t}
					gcmd<int32_t> pro;
					pro.id = id;
					pro.m_operator = "pay_responce";
					pro.m_data = rechange(lpay.m_orderid, lpay.m_rechargeid, false, true);
				};
			lcmd["gmrechange"] = [this](int id, ngl::ojson& aos)
				{
					int32_t lrechargeid;
					if (aos.read("data", lrechargeid) == false)
						return;

					std::string lorder;
					createorder(lorder, lrechargeid);
					
					// 返回 {"data":int32_t}
					gcmd<int32_t> pro;
					pro.id = id;
					pro.m_operator = "rechange_responce";
					pro.m_data = rechange(lorder, lrechargeid, true, true);
				};
			lcmd["rechange"] = [this](int id, ngl::ojson& aos)
				{//actor_role::loginpay() callback
					prorechange lrechange;
					if (aos.read("data", lrechange) == false)
						return;

					rechange(lrechange.m_orderid, lrechange.m_rechargeid, false, true);
				};
		}

		auto itor = lcmd.find(loperator);
		if (itor == lcmd.end())
		{
			LogLocalError("GM actor_role operator[%] ERROR", loperator);
			return true;
		}
		itor->second(adata.m_data->identifier(), lojson);
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

	bool actor_role::timer_handle(message<timerparm>& adata)
	{
		return true;
	}
}//namespace ngl