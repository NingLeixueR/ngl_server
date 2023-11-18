#include "actor_manage_activity.h"
#include "net.pb.h"
#include "ttab_calendar.h"

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
		type_roleitems::init(
			[](std::map<i64_actorid, roleitem>& amap, pbdb::db_brief& ainfo)
			{
				roleitem& rinfo = amap[ainfo.m_id()];
				rinfo.m_info = ainfo;
			}
		);
	}


	void actor_manage_activity::actor_register()
	{
		// 定时器
		actor::register_timer<actor_manage_activity>(&actor_manage_activity::timer_handle);
		// 协议注册
		register_actor<EPROTOCOL_TYPE_CUSTOM, actor_manage_activity>(
			false
			, null<actor_protocol_open_activity>
			, null<actor_protocol_close_activity>
			);
	}

	void actor_manage_activity::init()
	{
		timerparm tparm;
		make_timerparm::make_interval(tparm, 1);
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


	bool actor_manage_activity::open_activity(i64_actorid aactivityid)
	{
		activity** lpactivity = tools::findmap(m_allactivity, aactivityid);
		if (lpactivity == nullptr)
			return true;
		return (*lpactivity)->start();
	}

	bool actor_manage_activity::close_activity(i64_actorid aactivityid)
	{
		activity** lpactivity = tools::findmap(m_allactivity, aactivityid);
		if (lpactivity == nullptr)
			return true;
		(*lpactivity)->close();
		m_allactivity.erase(aactivityid);
		const pbdb::db_activity* ldb = m_db.get_constactivity(aactivityid);
		if (ldb == nullptr)
		{
			return true;
		}
		int lcount = ldb->m_datas_size();
		if (lcount == 0)
		{
			return true;
		}
		const pbdb::db_activity_data& litem = ldb->m_datas(lcount - 1);

		init_activity(aactivityid);
		start_activity(aactivityid);
		return true;
	}

	bool actor_manage_activity::handle(i32_threadid athread, const std::shared_ptr<pack>& apack, actor_protocol_open_activity& adata)
	{
		return open_activity(adata.m_activityid);
	}

	bool actor_manage_activity::handle(i32_threadid athread, const std::shared_ptr<pack>& apack, actor_protocol_close_activity& adata)
	{
		return close_activity(adata.m_activityid);
	}

	void actor_manage_activity::init_activity(int64_t aactivityid)
	{
		manage_csv<tab_activity>* lpcsv = allcsv::get<manage_csv<tab_activity>>();
		if (lpcsv == nullptr)
			return;
		const pbdb::db_activity* ldb = m_db.get_constactivity(aactivityid);
		if (ldb == nullptr)
		{
			tab_activity* tab = lpcsv->find(aactivityid);
			if (tab == nullptr)
			{
				LogLocalError("actor_manage_activity::init_activity() actorid=% tab_activitynot find!!!");
				return;
			}
			if (int32_t lbeg = 0, lend = 0; ttab_calendar::get(tab->m_calendarid, 0, lbeg, lend))
			{
				m_db.init_data(tab->m_id, lbeg, lend);
			}
		}
	}

	void actor_manage_activity::start_activity(int64_t aactivityid)
	{
		manage_csv<tab_activity>* lpcsv = allcsv::get<manage_csv<tab_activity>>();
		if (lpcsv == nullptr)
			return;
		int32_t lnow = localtime::gettime();
		const pbdb::db_activity* ldb = m_db.get_constactivity(aactivityid);
		if (ldb == nullptr)
		{
			return;
		}
		int lcount = ldb->m_datas_size();
		if (lcount == 0)
		{
			return;
		}
		const pbdb::db_activity_data& litem = ldb->m_datas(lcount-1);

		if (lnow > litem.m_end() && litem.m_start() == false)
		{// 活动没开启并且已过关闭时间
			// 活动下次执行时间
			tab_activity* tab = lpcsv->find(aactivityid);
			if (tab == nullptr)
				return;
			if (int32_t lbeg = 0, lend = 0; ttab_calendar::get(tab->m_calendarid, litem.m_end(), lbeg, lend))
			{
				m_db.init_data(aactivityid, lbeg, lend);
			}
			else
				return;
		}

		if (lnow < litem.m_beg() && litem.m_start() == false)
		{// 活动没开启并且未到开启时间 
			// 定时开启逻辑 定时关闭
			int64_t lactivity = ldb->m_id();
			post_timer(litem.m_beg(), [lactivity](wheel_node* anode)
				{
					actor::static_send_actor(
						actor_guid::make_self(ACTOR_ACTIVITY_MANAGE)
						, actor_guid::make()
						, std::make_shared<actor_protocol_open_activity>(lactivity)
					);
				});
			return;
		}

		

		if (lnow > litem.m_end() && litem.m_start() == true)
		{// 活动已开启并且已过关闭时间  
			// 关闭活动逻辑
			close_activity(ldb->m_id());
			return;
		}

		if (lnow > litem.m_beg() && lnow < litem.m_end() && litem.m_start() == false)
		{// 活动没开启并且已过开启时间 
			// 开启逻辑
			open_activity(ldb->m_id());
			return;
		}

		if (lnow > litem.m_beg() && lnow < litem.m_end() && litem.m_start() == true && litem.m_finish() == false)
		{// 活动已开启并且未关闭未到关闭时间 
			// 定时关闭逻辑
			int64_t lactivity = ldb->m_id();
			post_timer(litem.m_end(), [lactivity](wheel_node* anode)
				{
					actor::static_send_actor(
						actor_guid::make_self(ACTOR_ACTIVITY_MANAGE)
						, actor_guid::make()
						, std::make_shared<actor_protocol_close_activity>(lactivity)
					);
				});
			return;
		}

	}

	void actor_manage_activity::loaddb_finish(bool adbishave) 
	{
		int32_t lnow = localtime::gettime();
		manage_csv<tab_activity>* lpcsv = allcsv::get<manage_csv<tab_activity>>();
		if (lpcsv == nullptr)
			return;
		for (auto& [_id, _tab] : lpcsv->tablecsv)
		{
			init_activity(_id);
		}

		// ### 数据加载完成
		for (std::pair<const actor_guid, data_modified<pbdb::db_activity>>& item : m_db.data())
		{
			start_activity(item.first);
		}
	}

	bool actor_manage_activity::timer_handle(i32_threadid athread, const std::shared_ptr<pack>& apack, timerparm& adata)
	{
		if (adata.m_type != timerparm::ET_INTERVAL_SEC)
			return true;
		return true;
	}

}