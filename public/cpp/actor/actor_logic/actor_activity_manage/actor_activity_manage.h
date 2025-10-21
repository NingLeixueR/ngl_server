#pragma once

#include "actor_manage.h"
#include "actor_create.h"
#include "events_logic.h"
#include "ndb_modular.h"
#include "nsp_server.h"
#include "nsp_write.h"
#include "nsp_read.h"
#include "manage_csv.h"
#include "activitydb.h"
#include "ndbclient.h"
#include "nprotocol.h"
#include "activity.h"
#include "ntimer.h"
#include "net.pb.h"
#include "net.h"

#include "db_manage.h"
#include "db_data.h"
#include "db_pool.h"
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

		virtual void loaddb_finish(bool adbishave);

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
		bool handle(const message<np_arg_null>&);
		bool handle(const message<mforward<np_operator_task_response>>& adata);
		bool handle(const message<np_eevents_logic_rolelogin>& adata);
		bool handle(const message<np_eevents_logic_rolevaluechange>& adata);
		bool handle(const message<np_get_rank_response>& adata);
	};
}//namespace ngl
