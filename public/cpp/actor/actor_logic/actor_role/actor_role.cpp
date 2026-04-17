/*
* Copyright (c) [2020-2025] NingLeixueR
* 
* Project name: ngl_server
* Project URL: https://github.com/NingLeixueR/ngl_server
* 
* This file is part of the ngl_server project and is distributed under the MIT License.
* You may use, modify, and distribute this project under the license, including commercial use,
* but you must retain the original copyright and license notice.
* 
* For license details, see the LICENSE file in the project root:
* https://github.com/NingLeixueR/ngl_server/blob/main/LICENSE
*/
// File overview: Implements player role actor construction and sub-module initialization.


#include "actor/actor_logic/actor_events/actor_events.h"
#include "actor/actor_logic/actor_role/logic/bag/drop.h"
#include "actor/actor_logic/actor_role/actor_role.h"
#include "actor/actor_logic/actor_gm/gcmd.h"
#include "actor/actor_base/core/nregister.h"
#include "actor/actor_logic/nforward.h"
#include "actor/generated/pb/net.pb.h"
#include "actor/tab/ttab_specialid.h"
#include "actor/tab/ttab_recharge.h"
#include "tools/tools/tools_curl.h"

namespace ngl
{
	/**
	 * Constructor for actor_role.
	 * Initializes the role actor with specified area, role ID, and gateway information.
	 *
	 * @param aarea The logical area/shard this role belongs to
	 * @param aroleid The unique role ID within the area
	 * @param adata Pointer to np_actorswitch_process_role containing gateway ID
	 */
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
				.m_timeout = 1 * tools::time::MILLISECOND,
				.m_broadcast = true,
			})
		, m_gatewayid(((np_actorswitch_process_role*)(adata))->m_gatewayid)
	{
	}

	/**
	 * Get the actor type for role actors.
	 * @return ACTOR_ROLE enum value
	 */
	ENUM_ACTOR actor_role::actor_type()
	{
		return ACTOR_ROLE;
	}

	/**
	 * Generate a full actor ID from a role data ID.
	 * @param adata The role data ID
	 * @return The complete actor ID (GUID)
	 */
	i64_actorid actor_role::actorid(int32_t adata)
	{
		return nguid::make(actor_type(), tab_self_area, adata);
	}

	/**
	 * Initialize role actor components.
	 * Sets up info, bag, task, and key-value storage modules.
	 */
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

	/**
	 * Cleanup hook called before the actor is removed.
	 * Triggers offline events, exits NSP synchronization, and cleans up drop system.
	 */
	void actor_role::erase_actor_before()
	{
		// Trigger role offline event
		np_eevents_logic_roleoffline lparm;
		lparm.m_actorid = id_guid();
		actor_events_logic::trigger_event(lparm);

		// Exit NSP brief synchronization
		tdb_brief::nsp_cwrite<actor_role>::instance(id_guid()).exit();

		// Clean up drop system
		m_drop.exit();
	}

	/**
	 * Reset and update the daily login time.
	 * Checks if this is the first login of the day and updates task progress.
	 */
	void actor_role::reset_logintime()
	{
		time_t lloginutc = 0;
		time_t lnow = tools::time::gettime();
		bool isloginutc = false;
		// Check if last login was on a different day
		if (m_rolekv.get_value("base", { "loginutc" }, lloginutc))
		{
			if (tools::time::issameday(lnow, lloginutc) == false)
			{
				isloginutc = true;
			}
		}
		else
		{
			isloginutc = true;
		}

		// Update login time and task progress for new day
		if (isloginutc)
		{
			m_rolekv.set_value("base", { "loginutc" }, lnow);
			static_task::update_change(this, ETaskRoleLogin, 1);
		}
	}

	/**
	 * Callback when database loading finishes for this actor.
	 * Handles initialization after DB load and triggers login completion.
	 *
	 * @param atype The database table type that finished loading
	 * @param astat The load status (success, create, or failure)
	 */
	void actor_role::loaddb_finish(pbdb::ENUM_DB atype, enum_dbstat astat)
	{
		if (atype == pbdb::ENUM_DB::ENUM_DB_ALL)
		{
			log_error()->print("actor_role###loaddb_finish#[{}]", guid());
		}

		// Initialize after all DB tables are loaded
		if (atype == pbdb::ENUM_DB_ALL)
		{
			// Set up NSP synchronization for brief data
			std::set<i32_fieldnumber> lfieldset;
			tdb_brief::nsp_cwrite<actor_role>::instance_writepart(this, {}, tools::pb_field::field_number<pbdb::db_brief>(lfieldset, "mbase"), {}, { id_guid() });
			tdb_brief::nsp_cwrite<actor_role>::instance(id_guid()).set_changedatafun([this, astat](int64_t, const pbdb::db_brief&, bool afirstsynchronize)
				{
					// Trigger login finish on first synchronization
					if (afirstsynchronize && astat == enum_dbstat_success)
					{
						login_finish();
					}
				}
			);
			// Initialize drop system
			m_drop.init(this, {});
		}

		// Handle new role creation case
		if (astat == enum_dbstat_create && atype == pbdb::ENUM_DB::ENUM_DB_ALL)
		{
			pbnet::PROBUFF_NET_ROLE_NOT_CREATE pro;
			pro.set_mroleid(id_guid());
			actor::send_client(pro);
			return;
		}
	}

	/**
	 * Hook called after each message is handled.
	 * Synchronizes bag and attribute changes to the client.
	 */
	void actor_role::handle_after(handle_pram&)
	{
		// Sync bag changes to client
		m_bag.sync_client();
		// Sync attribute changes if modified
		if (m_attribute.sync())
		{
			m_attribute.set_sync(false);
			pbnet::PROBUFF_NET_SYNC_ATTRIBUTE pro;
			m_attribute.topb(pro);
			send_client(id_guid(), pro);
		}
		// Trigger NSP brief data change notification
		tdb_brief::nsp_cwrite<actor_role>::change(id_guid());
	}

	/**
	 * Complete the login process after all data is loaded and synchronized.
	 * Sends initial data to client and triggers login events.
	 */
	void actor_role::login_finish()
	{
		log_error()->print("actor_role###login_finish#[{}]", guid());
		// Send all role data to client
		sync_data_client();
		//m_info.sync_actor_brief();
		// Process login payment/rewards
		loginpay();

		// Trigger role login event
		np_eevents_logic_rolelogin lparm;
		lparm.m_actorid = id_guid();
		actor_events_logic::trigger_event(lparm);

		// Update daily login tracking
		reset_logintime();
	}

	/**
	 * Register message handlers for actor_role.
	 * Sets up protocol handlers and forwarding rules.
	 */
	void actor_role::nregister()
	{
		// Register timer handler
		actor::register_timer<actor_role>(&actor_role::timer_handle);

		// Register message handlers
		register_handle<actor_role
			, np_actor_disconnect_close
			, mforward<np_gm>
			, mforward<np_operator_task>
			, np_example_actorid
			, pbnet::PROBUFF_NET_ROLE_SYNC
			, pbnet::PROBUFF_NET_TASK_RECEIVE_AWARD
			, pbnet::PROBUFF_NET_ROLE_CREATE
		>();

		// Set up client-to-gateway forwarding
		nforward::c2g();
	}

	/**
	 * Get the gateway server ID for this role.
	 * @return The gateway server ID
	 */
	i32_serverid actor_role::get_getwayserverid()
	{
		return m_gatewayid;
	}

	/**
	 * Send an HTTP GET request for GM (Game Master) operations.
	 *
	 * @param aurl The URL to request
	 * @param aparm The query parameters
	 * @param acall Callback function to handle the response
	 */
	void actor_role::requestgm(const char* aurl, const std::string& aparm, const std::function<void(int, tools::http_parm&)>& acall)
	{
		auto lhttp = tools::curl::http();
		tools::curl::set_mode(lhttp, tools::ENUM_MODE_HTTP);
		tools::curl::set_type(lhttp, tools::ENUM_TYPE_GET);
		tools::curl::set_url(lhttp, aurl);
		tools::curl::set_param(lhttp, aparm);
		tools::curl::set_callback(lhttp, acall);
		tools::curl::send(lhttp);
	}

	void actor_role::loginpay()
	{
		requestgm(std::format("{}/pay/pay_login.php", sysconfig::gmurl()).c_str(), std::format("roleid={}", id_guid()), [this](int, tools::http_parm& ahttp)
			{
				if (ahttp.m_recvdata.empty())
				{
					return;
				}
				log_error()->print("actor_role::loginpay curl callback [{}]", ahttp.m_recvdata);
				try
				{
					ncjson ltempjson(ahttp.m_recvdata.c_str());
					if (ltempjson.parsecheck() == false)
					{
						log_error()->print("actor_role::loginpay curl callback fail");
						return;
					}
					std::string lorderid;
					int32_t lrechargeid = -1;
					int64_t lroleid = -1;
					if (!njson::pop(ltempjson, { "orderid", "rechargeid", "roleid"}, lorderid, lrechargeid, lroleid))
					{
						return;
					}

					auto lpayhttp = tools::curl::http();

					std::string lquestparm;
					tools::curl::param(lquestparm, "orderid", lorderid.c_str());
					tools::curl::param(lquestparm, "rechargeid", lrechargeid);
					tools::curl::param(lquestparm, "roleid", lroleid);
					requestgm((std::format("{}/pay/pay_login_stat.php", sysconfig::gmurl())).c_str(), lquestparm, [this](int32_t, tools::http_parm& ahttp)
						{
							log_error()->print("actor_role::loginpay curl callback [{}]", ahttp.m_recvdata);

							try
							{
								ncjson ltempjson(ahttp.m_recvdata.c_str());
								if (ltempjson.parsecheck() == false)
								{
									log_error()->print("actor_role::loginpay curl callback fail");
									return;
								}
								int32_t lstat = -1;
								if (!njson::pop(ltempjson, { "orderid" }, lstat))
								{
									return;
								}
								if (lstat != 0)
								{
									return;
								}
								int32_t lrechargeid = -1;
								if (!njson::pop(ltempjson, { "rechargeid" }, lrechargeid))
								{
									return;
								}
								std::string lorderid;
								if (!njson::pop(ltempjson, { "orderid" }, lorderid))
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
		MODIFIED_RETURN_CONST(lpdinfo, m_info.get());
		MODIFIED_RETURN_CONST(lpdbag, m_bag.get());
		MODIFIED_RETURN_CONST(lpdtask, m_task.get());
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
		aorder = std::format("{:05d}{:010d}{:010d}{:010d}{:02d}",area(), id(), arechargeid, tools::time::gettime(), billnoindex.fetch_add(1));
	}

	int32_t actor_role::rechange(std::string& aorderid, int32_t arechargeid, bool agm, bool areporting)
	{
		int32_t lstat = 0;
		int32_t lgold = 0;
		const tab_recharge* ltab = ttab_recharge::instance().tab(arechargeid);
		std::map<int, int> litems;
		if (ltab != nullptr)
		{
			std::string lsrc = std::format("recharge orderid={} rechargeid={}", aorderid, arechargeid);
			if (get_drop().use(ltab->m_dropid, 1, id_guid(), lsrc, &litems))
			{
				lgold += ltab->m_gold;
				lgold += ltab->m_bonus;
				if (is_first_recharge(arechargeid))
				{
					lgold += ltab->m_firstbonus;
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
			auto lhttp = tools::curl::http();
			tools::curl::set_mode(lhttp, tools::ENUM_MODE_HTTP);
			tools::curl::set_type(lhttp, tools::ENUM_TYPE_GET);
			std::string lurl = std::format("{}/pay/pay_update.php", sysconfig::gmurl());
			tools::curl::set_url(lhttp, lurl);

			std::string lparm = std::format("orderid={}&gm={}&roleid={}&stat={}",aorderid, (agm ? 1 : 0), id_guid(), lstat);

			tools::curl::set_param(lhttp, lparm.c_str());
			tools::curl::send(lhttp);
		}

		if (lgold > 0 || lstat == 0)
		{
			pbnet::PROBUFF_NET_DELIVER_GOODS_RECHARGE cpro;
			cpro.set_mrechargeid(arechargeid);
			cpro.set_morderid(aorderid);
			cpro.set_mgold(lgold);
			auto* litemmap = cpro.mutable_mitems();
			for (const auto& lpair : litems)
			{
				(*litemmap)[lpair.first] = lpair.second;
			}
			send_client(id_guid(), cpro);
		}	

		return lstat;
	}

		bool actor_role::is_first_recharge(int32_t arechargeid)
		{
			MODIFIED_RETURN_CONST(lpdinfoconst, m_info.get(), false);
			for (auto& recharge : lpdinfoconst->mrecharge())
			{
				if (recharge.mrechargeid() == arechargeid)
			{
				return false;
			}
		}
		return true;
	}

		bool actor_role::forward_before([[maybe_unused]] const pbnet::PROBUFF_NET_FAMIL_CREATE& adata)
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

		bool actor_role::timer_handle([[maybe_unused]] const message<np_timerparm>& adata)
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
			lpbriefbase->set_mcreateutc(static_cast<int32_t>(tools::time::gettime()));
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
