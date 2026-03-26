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
// File overview: Implements logic for actor activity manage.


#include "actor/actor_logic/actor_activity_manage/actor_activity_manage.h"
#include "actor/actor_logic/actor_activity_manage/activity.h"
#include "actor/actor_logic/actor_keyvalue/actor_keyvalue.h"
#include "actor/actor_logic/actor_ranklist/actor_ranklist.h"
#include "actor/actor_logic/actor_brief/actor_brief.h"
#include "actor/tab/ttab_activityopenserver.h"
#include "actor/tab/ttab_activityalways.h"
#include "actor/generated/pb/net.pb.h"

namespace ngl
{
	actor_activity_manage::actor_activity_manage() :
		actor(
			actorparm
			{
				.m_parm
				{
					.m_type = ACTOR_ACTIVITY_MANAGE,
					.m_area = tab_self_area,
					//.m_id = nconfig.tid(),
					.m_manage_dbclient = true,
				},
				.m_weight = 0x7fffffff,
				.m_broadcast = true
			})
	{
	}

	ENUM_ACTOR actor_activity_manage::actor_type()
	{
		return ACTOR_ACTIVITY_MANAGE;
	}

	i64_actorid actor_activity_manage::actorid()
	{
		return nguid::make(actor_type(), tab_self_area, nguid::none_actordataid());
	}

	void actor_activity_manage::init()
	{
		// Bind the DB structure: DB.set(this);
		m_activitydb.set(this);
		m_activitytimedb.set(this);
	}

	void actor_activity_manage::erase_actor_before()
	{
		tdb_brief::nsp_cwrite<actor_activity_manage>::instance(id_guid()).exit();
		tdb_keyvalue::nsp_cread<actor_activity_manage>::instance(id_guid()).exit();

		m_drop.exit();
	}

	void actor_activity_manage::handle_after(handle_pram&)
	{
		tdb_brief::nsp_cwrite<actor_activity_manage>::change(id_guid());
	}

	void actor_activity_manage::loaddb_finish(pbdb::ENUM_DB atype, enum_dbstat astat)
	{
		if (atype != pbdb::ENUM_DB::ENUM_DB_ALL)
		{
			log_error()->print("actor_activity_manage::loaddb_finish [{}:{}]", (int32_t)atype, (int32_t)astat);
			return;
		}
		std::set<i32_fieldnumber> lfieldset;
		pb_field::field_number<pbdb::db_brief>(lfieldset, "mid", "mactivityvalues");
		tdb_brief::nsp_cwrite<actor_activity_manage>::instance_writeall(this, {}, lfieldset);

		tdb_keyvalue::nsp_cread<actor_activity_manage>::instance_readall(this, {});

		tdb_activitytimes::nsp_ser::init(&m_activitytimedb);

		m_drop.init(this, {});

		actor_events_logic::event_func(actorid(), eevents_logic_rolelogin, eevents_logic_rolelevelchange, eevents_logic_rolegoldchange);

		// Loadinitializeactivity
		int32_t lnow = (int32_t)localtime::gettime();
		//std::map<nguid, data_modified<TDATA>>&
		for (auto& lpair : m_activitytimedb.data())
		{
			const nguid& lguid = lpair.first;
			const tab_activity* ltab = ttab_activity::instance().tab(nguid::actordataid(lguid));
			if (ltab == nullptr)
			{
				continue;
			}
			MODIFIED_CONTINUE_CONST(lpactivitytimesconst, lpair.second);
			int32_t lbeg = lpactivitytimesconst->mbeg();
			int32_t lduration = lpactivitytimesconst->mduration();
			start_activity(lguid, lbeg, lduration);
		}

		ttab_activity::instance().foreach([&](tab_activity& atab)
			{
				i64_actorid lactoractivityid = nguid::make(ACTOR_ACTIVITY_MANAGE, nguid::none_area(), atab.m_id);
				if (!m_activitys.contains(lactoractivityid))
				{
					if (atab.m_open == EActivityOpen::EActivityOpenAlways)
					{//ttab_activityalways
						const tab_activityalways* ltabalways = ttab_activityalways::instance().tab(atab.m_id);
						if (ltabalways == nullptr)
						{
							start_activity(lactoractivityid, lnow, -1);
							return;
						}
						if (ltabalways->m_type == EActivityAlways::EActivityAlwaysWeek)
						{
							int32_t lbeg = (int32_t)localtime::getweekday(lnow, ltabalways->m_wbday == 7 ? 0 : ltabalways->m_wbday, ltabalways->m_wbhour, ltabalways->m_wbminute, ltabalways->m_wbsecond);
							int32_t lend = (int32_t)localtime::getweekday(lnow, ltabalways->m_weday == 7 ? 0 : ltabalways->m_weday, ltabalways->m_wehour, ltabalways->m_weminute, ltabalways->m_wesecond);
							start_activity(atab.m_id, lbeg, lend - lbeg);
							return;
						}
						else if (ltabalways->m_type == EActivityAlways::EActivityAlwaysMonth)
						{
							std::pair<bool, time_t> lbeg = localtime::getmothday(lnow, ltabalways->m_mbday, ltabalways->m_mbhour, ltabalways->m_mbminute, ltabalways->m_mbsecond);
							if (lbeg.second)
							{
								return;
							}
							std::pair<bool, time_t> lend;
							int32_t lmeday = ltabalways->m_meday;
							auto lfun = [&lmeday, &lend, lnow, ltabalways]()
								{
									do
									{
										if (lmeday <= 0)
										{
											return false;
										}
										lend = localtime::getmothday(lnow, lmeday, ltabalways->m_mehour, ltabalways->m_meminute, ltabalways->m_mesecond);
										--lmeday;
									} while (!lend.first);
									return true;
								};
							if (!lfun())
							{
								return;
							}
							start_activity(lactoractivityid, (int32_t)lbeg.second, (int32_t)(lend.second - lbeg.second));
							return;
						}
						else if (ltabalways->m_type == EActivityAlways::EActivityAlwaysFixed)
						{
							int32_t lbeg = (int32_t)localtime::getsecond2time(lnow, ltabalways->m_fbhour, ltabalways->m_fbminute, ltabalways->m_fbsecond);
							int32_t lend = (int32_t)localtime::getsecond2time(lnow + ltabalways->m_fixedday * localtime::DAY_SECOND, ltabalways->m_fbhour, ltabalways->m_fbminute, ltabalways->m_fbsecond);
							start_activity(lactoractivityid, lbeg, lend - lbeg);
							return;
						}
					}
				}
			}
		);

		const pbdb::db_keyvalue* lkeyvalue = tdb_keyvalue::nsp_cread<actor_activity_manage>::instance(id_guid()).getconst(pbdb::db_keyvalue::open_server);
		if (lkeyvalue != nullptr)
		{
			static_cast<void>(tools::lexical_cast<int32_t>(lkeyvalue->mvalue()));
			ttab_activityopenserver::instance().foreach([&](tab_activityopenserver& atab)
				{
					if (!m_activitys.contains(atab.m_id))
					{
						//lopenserver
						int32_t lbeg = (atab.m_openday - 1) * localtime::DAY_SECOND + atab.m_openhour * localtime::HOUR_SECOND + atab.m_openminute * localtime::MINUTES_SECOND + atab.m_opensecond;
						int32_t lend = (atab.m_closeday - 1) * localtime::DAY_SECOND + atab.m_closehour * localtime::HOUR_SECOND + atab.m_closeminute * localtime::MINUTES_SECOND + atab.m_closesecond;
						start_activity(atab.m_id, lbeg, lend - lbeg);
					}
				}
			);
		}		
	}

	void actor_activity_manage::nregister()
	{
		// Timer.
		actor::register_timer<actor_activity_manage>(&actor_activity_manage::timer_handle);

		// Bind custom np_ messages.
		register_handle<actor_activity_manage
			, mforward<np_operator_task_response>
			, np_eevents_logic_rolelogin
			, np_eevents_logic_rolevaluechange
			, np_get_rank_response
		>(e_ready_all);
	}

	std::shared_ptr<activity>& actor_activity_manage::get_activity(i64_actorid aactivityid)
	{
		static std::shared_ptr<activity> lnull = nullptr;
		auto itor = m_activitys.find(aactivityid);
		if (itor == m_activitys.end())
		{
			return lnull;
		}
		return itor->second;
	}

	void actor_activity_manage::erase_activity(i64_actorid aactivityid)
	{
		m_activitys.erase(aactivityid);
	}

	void actor_activity_manage::start_activity(i64_actorid aactivityid, int32_t atime, int32_t aduration)
	{
		if (aduration < 0 && aduration !=-1)
		{
			log_error()->print("start_activity fail activityid=[{}] time=[{}] duration=[{}]", nguid(aactivityid), atime, aduration);
			return;
		}
		const tab_activity* ltab = ttab_activity::instance().tab(nguid::actordataid(aactivityid));
		if (ltab == nullptr)
		{
			return;
		}
		int32_t lnow = (int32_t)localtime::gettime();
		if (lnow < atime)
		{// Time startactivity
			post_timer(aactivityid, eactivity_start, atime, aduration);
			return;
		}
		if (lnow >= atime + aduration && aduration != -1)
		{// Closeactivity
			std::shared_ptr<activity>& lact = m_activitys.try_emplace(aactivityid).first->second;
			lact = activity::make(nguid::actordataid(aactivityid), atime, aduration, m_activitydb, m_activitytimedb);
			lact->init();
			lact->finish();
			lact = nullptr;
			return;
		}
		if ((lnow >= atime && lnow < atime + aduration) || (lnow >= atime && aduration == -1))
		{// Startactivity
			std::shared_ptr<activity>& lact = m_activitys.try_emplace(aactivityid).first->second;
			lact = activity::make(nguid::actordataid(aactivityid), atime, aduration, m_activitydb, m_activitytimedb);
			lact->start();
			lact->init();
			if (aduration > 0)
			{
				post_timer(aactivityid, eactivity_close, atime, aduration);
			}
		}
	}

	void actor_activity_manage::finish_activity(i64_actorid aactivityid)
	{
		auto itor = m_activitys.find(aactivityid);
		if (itor == m_activitys.end())
		{
			return;
		}
		itor->second->finish();
		erase_activity(aactivityid);
	}

	void actor_activity_manage::post_timer(i64_actorid aactivityid, etimerparm_activity atype, int32_t abeg, int32_t aduration)
	{
		auto ltimerparm = std::make_shared<np_timerparm>();
		int32_t lduration = 0;
		if (atype == eactivity_close)
		{
			lduration = (abeg + aduration) - (int32_t)localtime::gettime();
		}
		else if (atype == eactivity_start)
		{
			lduration = abeg - (int32_t)localtime::gettime();
		}

		if (lduration < 0)
		{
			lduration = 0;
		}
		
		make_timerparm::make_interval(*ltimerparm, lduration, 1);
		auto ltimerparm_activity = std::make_shared<timerparm_activity>();
		ltimerparm_activity->m_type = atype;
		ltimerparm_activity->m_activityid = aactivityid;
		ltimerparm_activity->m_beg = abeg;
		ltimerparm_activity->m_duration = aduration;
		timerparm_set_parm(ltimerparm.get(), ltimerparm_activity);		
		ntimer::addtimer(this, ltimerparm);
	}

	bool actor_activity_manage::timer_handle(const message<np_timerparm>& adata)
	{
		const np_timerparm* lrecv = adata.get_data();
		if (lrecv->m_type != E_ACTOR_TIMER::ET_INTERVAL_SEC)
		{
			return false;
		}
		std::shared_ptr<timerparm_activity> lpvparm = std::static_pointer_cast<timerparm_activity>(lrecv->m_parm);
		if (lpvparm == nullptr)
		{
			return false;
		}
		auto lpactivity = get_activity(lpvparm->m_activityid);
		if (lpactivity == nullptr)
		{
			return false;
		}
		if (lpvparm->m_type == eactivity_close)
		{
			finish_activity(lpvparm->m_activityid);
			return true;
		}
		else if (lpvparm->m_type == eactivity_start)
		{
			start_activity(lpvparm->m_activityid, lpvparm->m_beg, lpvparm->m_duration);
			return true;
		}

		return true;
	}
}// namespace ngl
