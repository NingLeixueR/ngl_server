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
#pragma once

#include "actor/actor_logic/actor_activity_manage/activitytimedb.h"
#include "actor/actor_logic/actor_activity_manage/activitydb.h"
#include "actor/actor_logic/actor_role/logic/bag/drop.h"
#include "actor/tab/ttab_activity.h"

namespace ngl
{
	class activity
	{
		activity(const activity&) = delete;
		activity& operator=(const activity&) = delete;

		static std::map<EActivity, activity*> m_activityall;
		virtual std::shared_ptr<activity> create(int32_t aactivityid, int32_t atime, int32_t aduration, activitydb& aactivitydb, activitytimedb& aactivitytimedb)
		{
			return std::make_shared<activity>(aactivityid, atime, aduration, aactivitydb, aactivitytimedb);
		}
	public:
		static std::shared_ptr<activity> make(int32_t aactivityid, int32_t atime, int32_t aduration, activitydb& aactivitydb, activitytimedb& aactivitytimedb)
		{
			const tab_activity* ltab = ttab_activity::instance().tab(aactivityid);
			auto itor = m_activityall.find(ltab->m_type);
			if (itor == m_activityall.end())
			{
				return nullptr;
			}
			return itor->second->create(aactivityid, atime, aduration, aactivitydb, aactivitytimedb);
		}
	protected:
		i64_actorid m_actorid									= 0;
		const tab_activity* m_tab								= nullptr;
		data_modified<pbdb::db_activity>* m_activity			= nullptr;
		data_modified<pbdb::db_activitytimes>* m_activitytimes	= nullptr;
		activitydb* m_activitydb								= nullptr;
		activitytimedb* m_activitytimedb						= nullptr;
	public:
		activity(int32_t activityid, int32_t atime, int32_t aduration, activitydb& aactivitydb, activitytimedb& aactivitytimedb);

		activity(EActivity atype);

		// # 获取活动类型
		EActivity type()
		{
			return m_tab->m_type;
		}

		// # 获取活动ID
		int64_t activityid()
		{
			return m_actorid;
		}

		// # 活动是否开启
		virtual bool is_start()
		{
			const pbdb::db_activitytimes* lpdata = m_activitytimes->getconst();
			if (lpdata == nullptr)
			{
				return false;
			}
			return lpdata->mstart();
		}

		// # 活动开启时间
		int32_t start_utc()
		{
			const pbdb::db_activitytimes* lpdata = m_activitytimes->getconst();
			if (lpdata == nullptr)
			{
				return -1;
			}
			return lpdata->mbeg();
		}
		
		// # 活动结束时间
		int32_t finish_utc()
		{
			const pbdb::db_activitytimes* lpdata = m_activitytimes->getconst();
			if (lpdata == nullptr)
			{
				return -1;
			}
			if (lpdata->mduration() == -1)
			{
				return -1;
			}
			return start_utc() + lpdata->mduration();
		}

		// # 此刻是活动第几天
		int32_t day()
		{
			return (int32_t)localtime::getspandays(localtime::gettime(), start_utc());
		}

		// # 获取活动表
		const tab_activity* tab()
		{
			return m_tab;
		}

		// # 是否支持排行榜
		bool is_rank()
		{
			return activityid() >= pbdb::eranklist::activity_lv;
		}
	public:
		// # 调用:活动开启
		virtual void start() 
		{
			const pbdb::db_activitytimes* lpconstdata = m_activitytimes->getconst();
			if (lpconstdata == nullptr)
			{
				return;
			}
			if (lpconstdata->mstart())
			{
				return;
			}
			log_error()->print("activity::start() activityid=[{}]", activityid());
			pbdb::db_activitytimes* lpdata = m_activitytimes->get();
			if (lpdata == nullptr)
			{
				return;
			}
			lpdata->set_mstart(true);
		}

		// # 调用:活动开启后和服务器重启
		virtual void init() 
		{
			log_error()->print("activity::init() activityid=[{}]", activityid());
		}

		// # 调用:玩家登陆
		virtual void rolelogin(i64_actorid aroleid);

		// # 调用:玩家开启活动成功
		void recv_task_response(i64_actorid aroleid, int32_t aindex, bool aisreceive)
		{
			pbdb::db_activity* lpdata = m_activity->get();
			if (lpdata == nullptr)
			{
				return;
			}
			if (aisreceive)
			{
				(*(*lpdata->mutable_mtask())[aroleid].mutable_mopen())[aindex] = true;
			}
			else
			{
				(*(*lpdata->mutable_mtask())[aroleid].mutable_mclose())[aindex] = true;
			}
		}

		// # 调用:玩家等级发生变化
		virtual void rolelevelchange(i64_actorid aroleid, int32_t abeforelevel, int32_t anowlevel);

		// # 调用:玩家金币发生变化
		virtual void rolegoldchange(i64_actorid aroleid, int32_t abeforegold, int32_t anowgold);

		// # db_brief.m_activityvalues发生变化后调用
		static void brief_activityvalues(i64_actorid aroleid);

		// # 调用:活动关闭
		virtual void finish() 
		{
			log_error()->print("activity::finish() activityid=[{}]", activityid());

			m_activitydb->erase(activityid());
			m_activitytimedb->erase(activityid());
		}
	};
}// namespace ngl