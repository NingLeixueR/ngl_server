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
#include "ttab_activityopenserver.h"
#include "actor_activity_manage.h"
#include "ttab_activityalways.h"
#include "actor_keyvalue.h"
#include "actor_ranklist.h"
#include "actor_brief.h"
#include "activity.h"
#include "net.pb.h"

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
					//.m_id = nconfig::m_tid,
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
		// 绑定DB结构:DB.set(this);
		m_activitydb.set(this);
		m_activitytimedb.set(this);

		tdb_brief::nsp_cwrite<actor_activity_manage>::instance_writeall(this, { pb_field::field_number<pbdb::db_brief>("mactivityvalues") });
		tdb_keyvalue::nsp_cread<actor_activity_manage>::instance_readall(this);

		tdb_activitytimes::nsp_ser::init(&m_activitytimedb);

		m_drop.init(this, {});
	}

	void actor_activity_manage::erase_actor_before()
	{
		tdb_brief::nsp_cwrite<actor_activity_manage>::instance(id_guid()).exit();
		tdb_keyvalue::nsp_cread<actor_activity_manage>::instance(id_guid()).exit();

		m_drop.exit();
	}

	void actor_activity_manage::handle_after(handle_pram&)
	{
		tdb_brief::nsp_cwrite<actor_activity_manage>::instance(id_guid()).change();
	}

	void actor_activity_manage::loaddb_finish(bool adbishave)
	{
		// 等待 actor_ranklist loaddb_finish
		wait_ready(actor_ranklist::actorid());

		actor_events_logic::tfun::func(
			actorid()
			, eevents_logic_rolelogin
			, eevents_logic_rolelevelchange
			, eevents_logic_rolegoldchange
		);

		// 加载初始化活动
		int32_t lnow = (int32_t)localtime::gettime();
		for (const auto& lpair : m_activitytimedb.data())
		{
			const tab_activity* ltab = ttab_activity::instance().tab(nguid::actordataid(lpair.first));
			if (ltab == nullptr)
			{
				continue;
			}
			data_modified_continue_getconst(lpactivitytimesconst, lpair.second);
			int32_t lbeg = lpactivitytimesconst->mbeg();
			int32_t lduration = lpactivitytimesconst->mduration();
			start_activity(lpair.first, lbeg, lduration);
		}

		auto ltabmap = ttab_activity::instance().tablecsv();
		if (ltabmap == nullptr)
		{
			tools::no_core_dump();
			return;
		}
		for (const auto& [activityid, tab] : *ltabmap)
		{
			i64_actorid lactoractivityid = tab2actor(ACTOR_ACTIVITY_MANAGE, activityid);
			if (!m_activitys.contains(lactoractivityid))
			{
				if (tab.m_open == EActivityOpen::EActivityOpenAlways)
				{//ttab_activityalways
					const tab_activityalways* ltabalways = ttab_activityalways::instance().tab(activityid);
					if (ltabalways == nullptr)
					{
						start_activity(lactoractivityid, lnow, -1);
						continue;
					}
					if (ltabalways->m_type == EActivityAlways::EActivityAlwaysWeek)
					{
						int32_t lbeg = (int32_t)localtime::getweekday(
							lnow,
							ltabalways->m_wbday==7?0: ltabalways->m_wbday, 
							ltabalways->m_wbhour, 
							ltabalways->m_wbminute, 
							ltabalways->m_wbsecond
						);
						int32_t lend = (int32_t)localtime::getweekday(
							lnow,
							ltabalways->m_weday == 7 ? 0 : ltabalways->m_weday,
							ltabalways->m_wehour,
							ltabalways->m_weminute,
							ltabalways->m_wesecond
						);
						start_activity(activityid, lbeg, lend - lbeg);
						continue;
					}
					else if (ltabalways->m_type == EActivityAlways::EActivityAlwaysMonth)
					{
						std::pair<bool, time_t> lbeg = localtime::getmothday(
							lnow,
							ltabalways->m_mbday,
							ltabalways->m_mbhour,
							ltabalways->m_mbminute,
							ltabalways->m_mbsecond
						);
						if (lbeg.second)
						{
							continue;
						}
						std::pair<bool, time_t> lend;
						int32_t lmeday = ltabalways->m_meday;
						auto lfun = [&lmeday,&lend, lnow, ltabalways]()
						{
								do
								{
									if (lmeday <= 0)
									{
										return false;
									}
									lend = localtime::getmothday(
										lnow,
										lmeday,
										ltabalways->m_mehour,
										ltabalways->m_meminute,
										ltabalways->m_mesecond
									);
									--lmeday;
								} while (!lend.first);
								return true;
						};
						if (!lfun())
						{
							continue;
						}
						start_activity(lactoractivityid, (int32_t)lbeg.second, (int32_t)(lend.second - lbeg.second));
						continue;
					}
					else if (ltabalways->m_type == EActivityAlways::EActivityAlwaysFixed)
					{
						int32_t lbeg = (int32_t)localtime::getsecond2time(
							lnow,
							ltabalways->m_fbhour,
							ltabalways->m_fbminute,
							ltabalways->m_fbsecond
						);
						int32_t lend = (int32_t)localtime::getsecond2time(
							lnow+ ltabalways->m_fixedday* localtime::DAY_SECOND,
							ltabalways->m_fbhour,
							ltabalways->m_fbminute,
							ltabalways->m_fbsecond
						);
						start_activity(lactoractivityid, lbeg, lend - lbeg);
						continue;
					}
				}
			}
		}

		const pbdb::db_keyvalue* lkeyvalue = tdb_keyvalue::nsp_cread<actor_activity_manage>::instance(id_guid()).getconst(pbdb::db_keyvalue::open_server);
		if (lkeyvalue != nullptr)
		{
			int32_t lopenserver = tools::lexical_cast<int32_t>(lkeyvalue->mvalue());
			auto ltabmap = ttab_activityopenserver::instance().tablecsv();
			if (ltabmap == nullptr)
			{
				tools::no_core_dump();
				return;
			}
			for (const auto& [activityid, tab] : *ltabmap)
			{
				if (!m_activitys.contains(activityid))
				{
					//lopenserver
					int32_t lbeg = (tab.m_openday - 1) * localtime::DAY_SECOND
						+ tab.m_openhour * localtime::HOUR_SECOND
						+ tab.m_openminute * localtime::MINUTES_SECOND
						+ tab.m_opensecond;
					int32_t lend = (tab.m_closeday - 1) * localtime::DAY_SECOND
						+ tab.m_closehour * localtime::HOUR_SECOND
						+ tab.m_closeminute * localtime::MINUTES_SECOND
						+ tab.m_closesecond;
					start_activity(activityid, lbeg, lend - lbeg);
				}
			}
		}		
	}

	void actor_activity_manage::nregister()
	{
		// 定时器
		actor::register_timer<actor_activity_manage>(&actor_activity_manage::timer_handle);

		// 绑定自定义np_消息
		register_handle<actor_activity_manage>::func<
			mforward<np_operator_task_response>
			, np_eevents_logic_rolelogin
			, np_eevents_logic_rolevaluechange
			, np_get_rank_response
		>(false);
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
		{//未来时间定时开启活动
			post_timer(aactivityid, eactivity_start, atime, aduration);
			return;
		}
		if (lnow >= atime + aduration && aduration != -1)
		{//关闭活动
			m_activitys[aactivityid] = activity::make(
				nguid::actordataid(aactivityid), atime, aduration, m_activitydb, m_activitytimedb
			);
			m_activitys[aactivityid]->init();
			m_activitys[aactivityid]->finish();
			m_activitys[aactivityid] = nullptr;
			return;
		}
		if ((lnow >= atime && lnow < atime + aduration) || (lnow >= atime && aduration == -1))
		{//开启活动
			m_activitys[aactivityid] = activity::make(
				nguid::actordataid(aactivityid), atime, aduration, m_activitydb, m_activitytimedb
			);
			m_activitys[aactivityid]->start();
			m_activitys[aactivityid]->init();
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
		if (adata.get_data()->m_type != E_ACTOR_TIMER::ET_INTERVAL_SEC)
		{
			return false;
		}
		const np_timerparm* lpparm = adata.get_data();
		std::shared_ptr<timerparm_activity> lpvparm = std::static_pointer_cast<timerparm_activity>(lpparm->m_parm);
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

	bool actor_activity_manage::handle(const message<np_arg_null>&)
	{
		return true;
	}
}// namespace ngl