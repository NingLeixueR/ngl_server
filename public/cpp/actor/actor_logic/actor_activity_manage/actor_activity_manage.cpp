#include "ttab_openserveractivity.h"
#include "actor_activity_manage.h"
#include "actor_keyvalue.h"
#include "ttab_calendar.h"
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
					//.m_id = nconfig::m_nodeid,
				},
				.m_weight = 0x7fffffff,
			})
	{
		tdb_brief::nsp_cli<actor_activity_manage>::getInstance().init(this, {});
		tdb_keyvalue::nsp_cli<actor_activity_manage>::getInstance().init(this, {});

		tdb_activitytimes::nsp_ser::init(&m_activitytimedb);
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
	}

	void actor_activity_manage::loaddb_finish(bool adbishave)
	{
		actor_events_logic::tfun::func(
			actorid()
			, eevents_logic_rolelogin
			, eevents_logic_rolelevelchange
			, eevents_logic_rolegoldchange
		);

		// 加载初始化活动
		int32_t lnow = localtime::gettime();
		for (const auto& [ activityid, activitytime] : m_activitytimedb.data())
		{
			const tab_activity* ltab = ttab_activity::tab(activityid);
			if (ltab == nullptr)
			{
				continue;
			}
			const pbdb::db_activitytimes& lactivitytimes = activitytime.getconst();
			if (lactivitytimes.m_finish())
			{
				m_activitys[activityid] = nullptr;
				
			}
			else
			{
				int32_t lbeg = lactivitytimes.m_beg();
				int32_t lduration = lactivitytimes.m_duration();
				start_activity(ltab->m_id, lbeg, lduration);
			}
		}

		for (const auto& [activityid, tab] : ttab_activity::tablecsv())
		{
			auto& lactivity = m_activitys[activityid];
			if (lactivity == nullptr)
			{
				if (tab.m_open == EActivityOpen::EActivityOpenAlways)
				{
					start_activity(activityid, lnow, -1);
				}
			}
		}

		//ttab_openserveractivity
		
		pbdb::db_keyvalue* lkeyvalue = tdb_keyvalue::nsp_cli<actor_activity_manage>::getInstance().get(pbdb::db_keyvalue::open_server);
		int32_t lopenserver = tools::lexical_cast<int32_t>(lkeyvalue->m_value());
		
		assert(lkeyvalue != nullptr);
		for (const auto& [activityid, tab] : ttab_openserveractivity::tablecsv())
		{
			auto& lactivity = m_activitys[activityid];
			if (lactivity == nullptr)
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
				start_activity(activityid, lbeg, lend- lbeg);
			}
		}
	}

	void actor_activity_manage::nregister()
	{
		// 定时器
		actor::register_timer<actor_activity_manage>(&actor_activity_manage::timer_handle);

		// 绑定自定义np_消息
		register_handle_custom<actor_activity_manage>::func<
			mforward<np_operator_task_response>
			, np_eevents_logic_rolelogin
			, np_eevents_logic_rolevaluechange
		>(false);

		// 绑定pb消息
		register_handle_proto<actor_activity_manage>::func<
		>(true);
	}

	std::shared_ptr<activity>& actor_activity_manage::get_activity(int64_t aactivityid)
	{
		static std::shared_ptr<activity> lnull = nullptr;
		auto itor = m_activitys.find(aactivityid);
		if (itor == m_activitys.end())
		{
			return lnull;
		}
		return itor->second;
	}

	void actor_activity_manage::erase_activity(int64_t aactivityid)
	{
		m_activitys.erase(aactivityid);
	}

	void actor_activity_manage::start_activity(int64_t aactivityid, int32_t atime, int32_t aduration)
	{
		const tab_activity* ltab = ttab_activity::tab(aactivityid);
		if (ltab == nullptr)
		{
			return;
		}
		int32_t lnow = localtime::gettime();
		if (atime < lnow)
		{
			post_timer(aactivityid, eactivity_start, atime, aduration);
		}
		else if (lnow >= atime + aduration)
		{
			m_activitys[aactivityid] = activity::make(
				aactivityid, atime, aduration, m_activitydb, m_activitytimedb
			);
			m_activitys[aactivityid]->init();
			m_activitys[aactivityid]->finish();
			m_activitys[aactivityid] = nullptr;
		}
		else
		{
			if (aduration > 0)
			{
				post_timer(aactivityid, eactivity_close, atime, aduration);
			}
		}
	}

	void actor_activity_manage::finish_activity(int64_t aactivityid)
	{
		auto itor = m_activitys.find(aactivityid);
		if (itor == m_activitys.end())
		{
			return;
		}
		itor->second->finish();
		erase_activity(aactivityid);
	}

	void actor_activity_manage::post_timer(int32_t aactivityid, etimerparm_activity atype, int32_t abeg, int32_t aduration)
	{
		auto ltimerparm = std::make_shared<np_timerparm>();
		int32_t lduration = 0;
		if (atype == eactivity_close)
		{
			lduration = (abeg + aduration) - localtime::gettime();
		}
		else if (atype == eactivity_start)
		{
			lduration = localtime::gettime() - abeg;
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