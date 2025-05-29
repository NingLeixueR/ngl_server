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

	void actor_activity_manage::add_activity(std::shared_ptr<activity>& aactivity)
	{
		m_activitys.insert(std::make_pair(aactivity->activityid(), aactivity));
	}

	void actor_activity_manage::erase_activity(int64_t aactivityid)
	{
		m_activitys.erase(aactivityid);
	}

	void actor_activity_manage::activity_start(int64_t aactivityid, int64_t atime, int32_t acalendarid)
	{
		if(m_activitys.contains(aactivityid))
		{
			return;
		}
		const auto tab = allcsv::tab<tab_activity>((int32_t)aactivityid);
		if (tab == nullptr)
		{
			return;
		}
		std::shared_ptr<activity> lpactivity = activity::make((int32_t)aactivityid, atime, m_activitydb, m_activitytimedb);
		add_activity(lpactivity);
		lpactivity->start();
	}

	void actor_activity_manage::activity_finish(int64_t aactivityid, int64_t atime, int32_t acalendarid)
	{
		auto itor = m_activitys.find(aactivityid);
		if (itor == m_activitys.end())
		{
			return;
		}
		itor->second->finish();
		erase_activity(aactivityid);
	}

	bool actor_activity_manage::timer_handle(const message<np_timerparm>& adata)
	{
		if (adata.get_data()->m_type != E_ACTOR_TIMER::ET_INTERVAL_SEC)
		{
			return false;
		}
		return true;
	}

	bool actor_activity_manage::handle(const message<np_arg_null>&)
	{
		return true;
	}
}// namespace ngl