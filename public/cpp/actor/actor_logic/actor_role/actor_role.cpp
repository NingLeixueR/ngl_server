/*
* Copyright (c) [2020-2025] NingLeixueR
* 
* 项目名称：ngl_server
* 项目地址：https://github.com/NingLeixueR/ngl_server
* 
* 本文件是 ngl_server 项目的一部分，遵循 MIT 开源协议发布。
* 您可以按照协议规定自由使用、修改和分发本项目，包括商业用途，
* 但需保留原始版权和许可声明。
* 
* 许可详情参见项目根目录下的 LICENSE 文件：
* https://github.com/NingLeixueR/ngl_server/blob/main/LICENSE
*/
#include "ttab_specialid.h"
#include "ttab_recharge.h"
#include "actor_events.h"
#include "manage_curl.h"
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
				.m_timeout = 1 * localtime::MILLISECOND,
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
		m_info.set(this);
		m_bag.set(this);
		m_task.set(this);
		m_rolekv.set(this);

		/*np_timerparm tparm;
		if (make_timerparm::make_interval(tparm, 2) == false)
		{
			log_error()->print("actor_role::init() make_timerparm::make_interval(tparm, 2) == false!!!");
			return;
		}
		set_timer(tparm);
		*/
	}

	void actor_role::erase_actor_before()
	{
		np_eevents_logic_roleoffline lparm;
		lparm.m_actorid = id_guid();
		actor_events_logic::trigger_event(lparm);

		tdb_brief::nsp_cwrite<actor_role>::instance(id_guid()).exit();

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

	void actor_role::loaddb_finish(pbdb::ENUM_DB atype, enum_dbstat astat)
	{
		if (atype == pbdb::ENUM_DB::ENUM_DB_ALL)
		{
			log_error()->print("actor_role###loaddb_finish#[{}]", guid());
		}
		
		if (atype == pbdb::ENUM_DB_ALL)
		{
			std::set<i32_fieldnumber> lfieldset;
			tdb_brief::nsp_cwrite<actor_role>::instance_writepart(this, {}, pb_field::field_number<pbdb::db_brief>(lfieldset, "mbase"), {}, { id_guid() });
			tdb_brief::nsp_cwrite<actor_role>::instance(id_guid()).set_changedatafun([this, astat](int64_t, const pbdb::db_brief&, bool afirstsynchronize)
				{
					if (afirstsynchronize && astat == enum_dbstat_success)
					{
						login_finish();
					}
				}
			);
			m_drop.init(this, {});
		}
		
		if (astat == enum_dbstat_create && atype == pbdb::ENUM_DB::ENUM_DB_ALL)
		{
			pbnet::PROBUFF_NET_ROLE_NOT_CREATE pro;
			pro.set_mroleid(id_guid());
			actor::send_client(pro);
			return;
		}
	}

	void actor_role::handle_after(handle_pram&)
	{
		m_bag.sync_client();
		if (m_attribute.sync())
		{
			m_attribute.set_sync(false);
			pbnet::PROBUFF_NET_SYNC_ATTRIBUTE pro;
			m_attribute.topb(pro);
			send_client(id_guid(), pro);
		}
		tdb_brief::nsp_cwrite<actor_role>::change(id_guid());
	}

	void actor_role::login_finish()
	{
		log_error()->print("actor_role###login_finish#[{}]", guid());
		sync_data_client();
		//m_info.sync_actor_brief();
		loginpay();

		np_eevents_logic_rolelogin lparm;
		lparm.m_actorid = id_guid();
		actor_events_logic::trigger_event(lparm);

		reset_logintime();
	}

	void actor_role::nregister()
	{
		actor::register_timer<actor_role>(&actor_role::timer_handle);

		register_handle<actor_role>::func<
			np_actor_disconnect_close
			, mforward<np_gm>
			, mforward<np_operator_task>
			, np_example_actorid
			, pbnet::PROBUFF_NET_ROLE_SYNC
			, pbnet::PROBUFF_NET_TASK_RECEIVE_AWARD
			, pbnet::PROBUFF_NET_ROLE_CREATE
		>(e_ready_all);

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
		requestgm(std::format("{}/pay/pay_login.php", sysconfig::gmurl()).c_str(), std::format("roleid={}", id_guid()), [this](int, http_parm& ahttp)
			{
				if (ahttp.m_recvdata.empty())
				{
					return;
				}
				log_error()->print("actor_role::loginpay curl callback [{}]", ahttp.m_recvdata);
				try
				{
					ncjson ltempjson(ahttp.m_recvdata.c_str());
					if (ltempjson.check() == false)
					{
						log_error()->print("actor_role::loginpay curl callback fail");
						return;
					}
					std::string lorderid;
					if (!njson::pop(ltempjson.json(), "orderid", lorderid))
					{
						return;
					}
					int32_t lrechargeid = -1;
					if (!njson::pop(ltempjson.json(), "rechargeid", lrechargeid))
					{
						return;
					}
					int64_t lroleid = -1;
					if (!njson::pop(ltempjson.json(), "roleid", lroleid))
					{
						return;
					}

					auto lpayhttp = ngl::manage_curl::make_http();

					std::string lquestparm;
					ngl::manage_curl::param(lquestparm, "orderid", lorderid.c_str());
					ngl::manage_curl::param(lquestparm, "rechargeid", lrechargeid);
					ngl::manage_curl::param(lquestparm, "roleid", lroleid);
					requestgm((std::format("{}/pay/pay_login_stat.php", sysconfig::gmurl())).c_str(), lquestparm, [this](int32_t, http_parm& ahttp)
						{
							log_error()->print("actor_role::loginpay curl callback [{}]", ahttp.m_recvdata);

							try
							{
								ncjson ltempjson(ahttp.m_recvdata.c_str());
								if (ltempjson.check() == false)
								{
									log_error()->print("actor_role::loginpay curl callback fail");
									return;
								}
								int32_t lstat = -1;
								if (!njson::pop(ltempjson.json(), "orderid", lstat))
								{
									return;
								}
								if (lstat != 0)
								{
									return;
								}
								int32_t lrechargeid = -1;
								if (!njson::pop(ltempjson.json(), "rechargeid", lrechargeid))
								{
									return;
								}
								std::string lorderid;
								if (!njson::pop(ltempjson.json(), "orderid", lorderid))
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
		pbnet::PROBUFF_NET_ROLE_SYNC_RESPONSE pro;
		data_modified_return_getconst(lpdinfo, m_info.get());
		data_modified_return_getconst(lpdbag, m_bag.get());
		data_modified_return_getconst(lpdtask, m_task.get());
		*pro.mutable_mrole()	= *lpdinfo;
		*pro.mutable_mbag()	= *lpdbag;
		*pro.mutable_mtask()	= *lpdtask;
		const pbdb::db_brief* lpbrief = tdb_brief::nsp_cwrite<actor_role>::instance(id_guid()).getconst(id_guid());
		if (lpbrief != nullptr)
		{
			*pro.mutable_mbrief() = *lpbrief;
		}
		send_client(id_guid(), pro);
	}

	void actor_role::createorder(std::string& aorder, int32_t arechargeid)
	{
		static std::atomic<int32_t> billnoindex = 0;
		aorder = std::format("{:05d}{:010d}{:010d}{:010d}{:02d}",area(), id(), arechargeid, localtime::gettime(), billnoindex.fetch_add(1));
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
			auto lhttp = ngl::manage_curl::make_http();
			ngl::manage_curl::set_mode(lhttp, ngl::ENUM_MODE_HTTP);
			ngl::manage_curl::set_type(lhttp, ngl::ENUM_TYPE_GET);
			std::string lurl = std::format("{}/pay/pay_update.php", sysconfig::gmurl());
			ngl::manage_curl::set_url(lhttp, lurl);

			std::string lparm = std::format("orderid={}&gm={}&roleid={}&stat={}",aorderid, (agm ? 1 : 0), id_guid(), lstat);

			ngl::manage_curl::set_param(lhttp, lparm.c_str());
			ngl::manage_curl::send(lhttp);
		}

		if (lgold > 0 || lstat == 0)
		{
			pbnet::PROBUFF_NET_DELIVER_GOODS_RECHARGE cpro;
			cpro.set_mrechargeid(arechargeid);
			cpro.set_morderid(aorderid);
			cpro.set_mgold(lgold);
			for (auto itor = litems.begin(); itor != litems.end(); ++itor)
			{
				(*cpro.mutable_mitems())[itor->first] = itor->second;
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
		pbnet::PROBUFF_NET_MSG_RESPONSE pro;
		pro.set_mmsg(amsg);
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

	void actor_role::create_init(const std::string& aname)
	{
		pbdb::db_brief* lpbrief = tdb_brief::nsp_cwrite<actor_role>::instance(id_guid()).add(id_guid());
		if (lpbrief == nullptr)
		{
			return;
		}
		pbdb::brief_base* lpbriefbase = lpbrief->mutable_mbase();
		if (lpbriefbase == nullptr)
		{
			return;
		}
		lpbriefbase->set_mlv(1);
		lpbriefbase->set_mcreateutc(localtime::gettime());
		lpbriefbase->set_mmoneygold(0);
		lpbriefbase->set_mmoneysilver(0);
		lpbriefbase->set_mname(aname);
		lpbriefbase->set_mnotalkutc(0);
		lpbriefbase->set_mvip(0);

		pbdb::db_role* lprole = m_info.get().get();
		if (lprole == nullptr)
		{
			return;
		}

		sync_data_client();
	}
}//namespace ngl