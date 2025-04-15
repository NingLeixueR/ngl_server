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
		std::set<i64_actorid> ldataid;
		
		nclient_brief::init((ENUM_ACTOR)nguid::type(actor_brief::actorid()), this, ldataid);
		std::set<i64_actorid> ldatakvid;
		nclient_keyvalue::init((ENUM_ACTOR)nguid::type(actor_keyvalue::actorid()), this, ldatakvid);
	}

	ENUM_ACTOR actor_activity_manage::actor_type()
	{
		return ACTOR_ACTIVITY_MANAGE;
	}

	i64_actorid actor_activity_manage::actorid()
	{
		return nguid::make(ACTOR_ACTIVITY_MANAGE, tab_self_area, nguid::none_actordataid());
	}

	void actor_activity_manage::add_activity(int64_t actorid, std::shared_ptr<activity>& activ)
	{
		auto itor = m_allactivity.insert(std::make_pair(actorid, activ));
		if (itor.second == false)
		{
			return;
		}
		if (activ->calendarid() < 0)
		{
			activ->post_timer();
		}
	}

	void actor_activity_manage::init()
	{
		np_timerparm tparm;
		if (make_timerparm::make_interval(tparm, 5) == false)
		{
			log_error()->print("actor_manage_activity::init() make_timerparm::make_interval(tparm, 5) == false!!!");
			return;
		}
		set_timer(tparm);
		m_db.set(this);
	}

	void actor_activity_manage::loaddb_finish(bool adbishave)
	{
	}

	int64_t post_timer(int32_t autc, const std::function<void(const wheel_node* anode)>& afun)
	{
		auto lms = (int32_t)(localtime::gettime() * 1000 - (autc * 1000));

		wheel_parm lparm
		{
			.m_ms = lms,
			.m_intervalms = nullptr ,
			.m_count = 1,
			.m_fun = [afun](const wheel_node* anode)
			{
				afun(anode);
			}
		};
		return twheel::wheel().addtimer(lparm);
	}

	void actor_activity_manage::activity_start(int64_t aactivityid, int64_t atime, int32_t acalendarid)
	{
		if (auto itor = m_allactivity.find(aactivityid); itor != m_allactivity.end())
		{
			return;
		}

		if (const auto tab = allcsv::tab<tab_activity>((int32_t)aactivityid); tab == nullptr)
		{
			return;
		}
		std::shared_ptr<activity> lactivity = activity::make(acalendarid, (int32_t)aactivityid, atime, m_db);
		add_activity(aactivityid, lactivity);
	}

	void actor_activity_manage::activity_finish(int64_t aactivityid, int64_t atime, int32_t acalendarid)
	{
		auto itor = m_allactivity.find(aactivityid);
		if (itor == m_allactivity.end())
		{
			return;
		}
		itor->second->finish();
	}

	bool actor_activity_manage::timer_handle(const message<np_timerparm>& adata)
	{
		if (adata.get_data()->m_type != E_ACTOR_TIMER::ET_INTERVAL_SEC)
		{
			return false;
		}

		return true;
	}

	void actor_activity_manage::nregister()
	{
		// 定时器
		actor::register_timer<actor_activity_manage>(&actor_activity_manage::timer_handle);
		// 协议注册
		register_handle_custom<actor_activity_manage>::func<
			np_actor_activity
		>(false);
	}
}// namespace ngl