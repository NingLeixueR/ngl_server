#include "ttab_specialid.h"
#include "ttab_recharge.h"
#include "actor_events.h"
#include "manage_curl.h"
#include "nsp_server.h"
#include "nsp_client.h"
#include "actor_role.h"
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
				.m_weight = 0x7fffffff,
				.m_timeout = 10000,
				.m_broadcast = true,
			})
		, m_gatewayid(((np_actorswitch_process_role*)(adata))->m_gatewayid)
	{
	}

	actor_role::~actor_role()
	{
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
			log_error()->print("actor_role::init() make_timerparm::make_interval(tparm, 2) == false!!!");
			return;
		}
		set_timer(tparm);
		*/

		
		tdb_brief::nsp_cli<actor_role>::instance(id_guid(), true).init_parts(this, { nguid::make_type(id_guid(), ACTOR_BRIEF) });
		tdb_brief::nsp_cli<actor_role>::instance(id_guid()).set_changedata_fun(
			[this](int64_t, const pbdb::db_brief&, bool afirstsynchronize)
			{
				if (afirstsynchronize)
				{// 数据完全加载
					login_finish();
				}
			});

		m_drop.init(this, {});
	}

	void actor_role::erase_actor_before()
	{
		np_eevents_logic_roleoffline lparm;
		lparm.m_actorid = id_guid();
		actor_events_logic::trigger_event(lparm);

		tdb_brief::nsp_cli<actor_role>::instance(id_guid()).exit();
		tdb_brief::nsp_cli<actor_role>::freensp(id_guid());

		m_drop.exit();
	}

	void actor_role::reset_logintime()
	{
		time_t lloginutc = 0;
		time_t lnow = localtime::gettime();
		bool isloginutc = false;
		if (m_rolekv.value("loginutc", lloginutc))
		{
			if (localtime::issameday(lnow, lloginutc) == false)
			{
				isloginutc = true;
			}
		}
		else
		{
			isloginutc = true;
		}
		if (isloginutc)
		{
			m_rolekv.set_value("loginutc", lnow);
			static_task::update_change(this, ETaskRoleLogin, 1);
		}
	}

	void actor_role::loaddb_finish(bool adbishave)
	{
		log_error()->print("actor_role###loaddb_finish#[{}]", guid());
	}

	void actor_role::login_finish()
	{
		log_error()->print("actor_role###login_finish#[{}]", guid());
		sync_data_client();
		//m_info.sync_actor_brief();
		loginpay();

		// # 登陆事件
		np_eevents_logic_rolelogin lparm;
		lparm.m_actorid = id_guid();
		actor_events_logic::trigger_event(lparm);

		reset_logintime();
	}

	void actor_role::nregister()
	{
		// 定时器
		actor::register_timer<actor_role>(&actor_role::timer_handle);

		// 绑定自定义np_消息
		register_handle<actor_role>::func<
			np_actor_disconnect_close
			, mforward<np_gm>
			, mforward<np_operator_task>
			, np_example_actorid
			, pbnet::PROBUFF_NET_ROLE_SYNC
			, pbnet::PROBUFF_NET_TASK_RECEIVE_AWARD
		>(false);

		nforward::c2g();
	}

	i32_serverid actor_role::get_getwayserverid()
	{
		return m_gatewayid;
	}


	void actor_role::requestgm(const char* aurl, const std::string& aparm, const std::function<void(int, http_parm&)>& acall)
	{
		auto lhttp = ngl::manage_curl::make_http();
		ngl::manage_curl::set_mode(lhttp, ngl::ENUM_MODE_HTTP);
		ngl::manage_curl::set_type(lhttp, ngl::ENUM_TYPE_GET);
		ngl::manage_curl::set_url(lhttp, aurl);

		ngl::manage_curl::set_param(lhttp, aparm);

		ngl::manage_curl::set_callback(lhttp, acall);
		ngl::manage_curl::send(lhttp);
	}

	void actor_role::loginpay()
	{
		// ### 检查是否有充值未发货
		requestgm("http://127.0.0.1:800/pay/pay_login.php", std::format("roleid={}", id_guid()), [this](int, http_parm& ahttp)
			{
				if (ahttp.m_recvdata.empty())
				{
					return;
				}
				log_error()->print("actor_role::loginpay curl callback [{}]", ahttp.m_recvdata);
				try
				{
					njson_read ltempjson(ahttp.m_recvdata.c_str());
					if (ltempjson.check() == false)
					{
						log_error()->print("actor_role::loginpay curl callback fail");
						return;
					}
					std::string lorderid;
					if (!njson::read(ltempjson, "orderid", lorderid))
					{
						return;
					}
					int32_t lrechargeid = -1;
					if (!njson::read(ltempjson, "rechargeid", lrechargeid))
					{
						return;
					}
					int64_t lroleid = -1;
					if (!njson::read(ltempjson, "roleid", lroleid))
					{
						return;
					}

					auto lpayhttp = ngl::manage_curl::make_http();

					std::string lquestparm;
					ngl::manage_curl::param(lquestparm, "orderid", lorderid.c_str());
					ngl::manage_curl::param(lquestparm, "rechargeid", lrechargeid);
					ngl::manage_curl::param(lquestparm, "roleid", lroleid);
					requestgm("http://127.0.0.1:800/pay/pay_login_stat.php", lquestparm, [this](int32_t, http_parm& ahttp)
						{
							log_error()->print("actor_role::loginpay curl callback [{}]", ahttp.m_recvdata);

							try
							{
								njson_read ltempjson(ahttp.m_recvdata.c_str());
								if (ltempjson.check() == false)
								{
									log_error()->print("actor_role::loginpay curl callback fail");
									return;
								}
								int32_t lstat = -1;
								if (!njson::read(ltempjson, "orderid", lstat))
								{
									return;
								}
								if (lstat != 0)
								{
									return;
								}
								int32_t lrechargeid = -1;
								if (!njson::read(ltempjson, "rechargeid", lrechargeid))
								{
									return;
								}
								std::string lorderid;
								if (!njson::read(ltempjson, "orderid", lorderid))
								{
									return;
								}

								const tab_recharge* ltab = ttab_recharge::instance().tab(lrechargeid);
								if (ltab == nullptr)
								{
									return;
								}

								rechange(lorderid, lrechargeid, false, true);
							}
							catch (...)
							{
							}
						});
				}
				catch (...) {}
			});
		
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
		data_modified_return_getconst(lpdinfo, m_info.get());
		data_modified_return_getconst(lpdbag, m_bag.get());
		data_modified_return_getconst(lpdtask, m_task.get());
		*pro->mutable_mrole()	= *lpdinfo;
		*pro->mutable_mbag()	= *lpdbag;
		*pro->mutable_mtask()	= *lpdtask;
		const pbdb::db_brief* lpbrief = tdb_brief::nsp_cli<actor_role>::instance(id_guid()).getconst(id_guid());
		if (lpbrief != nullptr)
		{
			*pro->mutable_mbrief() = *lpbrief;
		}
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
		int32_t lstat = 0;
		int32_t lgold = 0;
		tab_recharge* tab = allcsv::tab<tab_recharge>(arechargeid);
		std::map<int, int> litems;
		if (tab != nullptr)
		{
			std::string lsrc = std::format("recharge orderid={} rechargeid={}", aorderid, arechargeid);
			if(get_drop().use(tab->m_dropid, 1, id_guid(), lsrc, &litems))
			{
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
				lstat = 1;
			}
		}
		else
		{
			lstat = 2;
		}
		
		if (areporting && lstat == 0)
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

		if (lgold > 0 || lstat == 0)
		{
			auto cpro = std::make_shared<pbnet::PROBUFF_NET_DELIVER_GOODS_RECHARGE>();
			cpro->set_mrechargeid(arechargeid);
			cpro->set_morderid(aorderid);
			cpro->set_mgold(lgold);
			for (auto itor = litems.begin(); itor != litems.end(); ++itor)
			{
				(*cpro->mutable_mitems())[itor->first] = itor->second;
			}
			send_client(id_guid(), cpro);
		}	

		return lstat;
	}

	bool actor_role::is_first_recharge(int32_t arechargeid)
	{
		data_modified_return_getconst(lpdinfoconst, m_info.get(), false);
		int lcount = 0;
		for (const auto& item : lpdinfoconst->mrecharge())
		{
			if (item.mrechargeid() == arechargeid)
			{
				return false;
			}
		}
		return true;
	}

	bool actor_role::forward_before(const pbnet::PROBUFF_NET_FAMIL_CREATE& adata)
	{
		if (ttab_specialid::instance().m_createfamilconsume > m_info.gold())
		{
			return false;
		}
		m_info.change_gold(-ttab_specialid::instance().m_createfamilconsume);
		return true;
	}

	void actor_role::echo_msg(const char* amsg)
	{
		auto pro = std::make_shared<pbnet::PROBUFF_NET_MSG_RESPONSE>();
		pro->set_mmsg(amsg);
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