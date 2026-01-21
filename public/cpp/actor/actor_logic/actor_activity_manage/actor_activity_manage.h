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

#include "actor_manage.h"
#include "actor_create.h"
#include "events_logic.h"
#include "ndb_modular.h"
#include "ncsv.h"
#include "activitydb.h"
#include "db_manage.h"
#include "ndbclient.h"
#include "nprotocol.h"
#include "activity.h"
#include "db_data.h"
#include "db_pool.h"
#include "ntimer.h"
#include "net.pb.h"
#include "net.h"
#include "db.h"

namespace ngl
{
	enum etimerparm_activity
	{
		eactivity_null,
		eactivity_start,
		eactivity_close,
	};
	struct timerparm_activity
	{
		etimerparm_activity m_type = eactivity_null;
		i64_actorid m_activityid = 0;
		int32_t m_beg = 0;
		int32_t m_duration = 0;
	};

	class actor_activity_manage : 
		public actor
	{
		actor_activity_manage(const actor_activity_manage&) = delete;
		actor_activity_manage& operator=(const actor_activity_manage&) = delete;

		actor_activity_manage();

		std::map<i64_actorid, std::shared_ptr<activity>> m_activitys;
		drop<actor_activity_manage> m_drop;
	public:
		activitydb m_activitydb;
		activitytimedb m_activitytimedb;

		friend class actor_instance<actor_activity_manage>;
		static actor_activity_manage& instance()
		{
			return actor_instance<actor_activity_manage>::instance();
		}

		virtual ~actor_activity_manage() = default;

		static ENUM_ACTOR actor_type();

		static i64_actorid actorid();

		virtual void init();

		virtual void erase_actor_before();

		virtual void loaddb_finish(pbdb::ENUM_DB atype, enum_dbstat astat);

		static void nregister();

		//# 执行handle之后调用
		virtual void handle_after(handle_pram&);

		std::shared_ptr<activity>& get_activity(i64_actorid aactivityid);

		void erase_activity(i64_actorid aactivityid);

		void start_activity(i64_actorid aactivityid, int32_t atime, int32_t aduration);

		void finish_activity(i64_actorid aactivityid);

		void post_timer(i64_actorid aactivityid, etimerparm_activity atype, int32_t abeg, int32_t aduration);

		static drop<actor_activity_manage>& get_drop()
		{
			return instance().m_drop;
		}

		bool timer_handle(const message<np_timerparm>& adata);
		bool handle(const message<mforward<np_operator_task_response>>& adata);
		bool handle(const message<np_eevents_logic_rolelogin>& adata);
		bool handle(const message<np_eevents_logic_rolevaluechange>& adata);
		bool handle(const message<np_get_rank_response>& adata);
	};
}//namespace ngl
