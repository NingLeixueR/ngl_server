#include "actor_manage_activity.h"
#include "actor_keyvalue.h"
#include "ttab_calendar.h"
#include "actor_brief.h"
#include "activity.h"
#include "net.pb.h"

namespace ngl
{
	actor_manage_activity::actor_manage_activity() :
		actor(
			actorparm
			{
				.m_parm
				{
					.m_type = ACTOR_ACTIVITY_MANAGE,
					.m_area = ttab_servers::tab()->m_area,
					//.m_id = nconfig::m_nodeid,
				},
				.m_weight = 0x7fffffff,
			})
	{
		std::set<i64_actorid> ldataid;
		nclient_brief::init(actor_brief::actorid(), this, ldataid);
		std::set<i64_actorid> ldatakvid = 
		{ 
			pbdb::db_keyvalue_ekv_serveropentime 
		};
		nclient_keyvalue::init(actor_keyvalue::actorid(), this, ldatakvid);
	}

	void actor_manage_activity::nregister()
	{
		// 定时器
		actor::register_timer<actor_manage_activity>(&actor_manage_activity::timer_handle);
		// 协议注册
		register_handle_custom<actor_manage_activity>::func<
			np_actor_activity
		>(false);
	}

	void actor_manage_activity::init()
	{
		timerparm tparm;
		if (make_timerparm::make_interval(tparm, 5) == false)
		{
			log_error()->print("actor_manage_activity::init() make_timerparm::make_interval(tparm, 5) == false!!!");
			return;
		}
		set_timer(tparm);
		m_db.set(this);
	}

	int64_t post_timer(int32_t autc, const std::function<void(wheel_node* anode)>& afun)
	{
		int32_t lms = localtime::gettime()*1000 - (autc * 1000);

		wheel_parm lparm
		{
			.m_ms = lms,
			.m_intervalms = nullptr ,
			.m_count = 1,
			.m_fun = [afun](wheel_node* anode)
			{
				afun(anode);
			}
		};
		return twheel::wheel().addtimer(lparm);
	}

	void actor_manage_activity::activity_start(int64_t aactivityid, int64_t atime, int32_t acalendarid)
	{
		auto itor = m_allactivity.find(aactivityid);
		if (itor != m_allactivity.end())
		{
			return;
		}
		tab_activity* tab = allcsv::tab<tab_activity>(aactivityid);
		if (tab == nullptr)
		{
			return;
		}
		std::shared_ptr<activity> lactivity = activity::make(acalendarid, aactivityid, atime, m_db);
		add_activity(aactivityid, lactivity);
	}

	void actor_manage_activity::activity_finish(int64_t aactivityid, int64_t atime, int32_t acalendarid)
	{
		auto itor = m_allactivity.find(aactivityid);
		if (itor == m_allactivity.end())
		{
			return;
		}
		itor->second->finish();
	}
	
	bool actor_manage_activity::handle(message<np_actor_activity>& adata)
	{
		np_actor_activity& lrecv = *adata.get_data();
		for (i64_actorid item : lrecv.m_activityids)
		{
			if (lrecv.m_start)
			{
				activity_start(item, lrecv.m_time, lrecv.m_calendarid);
			}
			else
			{
				activity_finish(item, lrecv.m_time, lrecv.m_calendarid);
			}
		}
		return true;
	}

	void actor_manage_activity::loaddb_finish(bool adbishave) 
	{
	}

	bool actor_manage_activity::timer_handle(message<timerparm>& adata)
	{
		if (adata.get_data()->m_type != timerparm::ET_INTERVAL_SEC)
			return true;
		return true;
	}
}// namespace ngl