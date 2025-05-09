#pragma once

#include "actor_manage.h"
#include "actor_create.h"
#include "db_manage.h"
#include "db_data.h"
#include "db_pool.h"
#include "db.h"
#include "ndb_modular.h"
#include "nsp_server.h"
#include "nsp_client.h"
#include "manage_csv.h"
#include "activitydb.h"
#include "ndbclient.h"
#include "nprotocol.h"
#include "activity.h"
#include "ntimer.h"
#include "net.pb.h"
#include "net.h"

namespace ngl
{
	class actor_activity_manage : public actor
	{
		actor_activity_manage(const actor_activity_manage&) = delete;
		actor_activity_manage& operator=(const actor_activity_manage&) = delete;

		actor_activity_manage();

		activitydb m_db;
		std::map<int64_t, std::shared_ptr<activity>> m_allactivity;

		using nclient_brief		= tdb_brief::nsp_cli<actor_activity_manage>;
		using nclient_keyvalue	= tdb_keyvalue::nsp_cli<actor_activity_manage>;
	public:
		friend class actor_instance<actor_activity_manage>;
		static actor_activity_manage& getInstance()
		{
			return actor_instance<actor_activity_manage>::instance();
		}

		virtual ~actor_activity_manage() = default;

		static ENUM_ACTOR actor_type();

		static i64_actorid actorid();

		virtual void init();

		virtual void loaddb_finish(bool adbishave);

		static void nregister();

		void activity_start(int64_t aactivityid, int64_t atime, int32_t acalendarid);

		void activity_finish(int64_t aactivityid, int64_t atime, int32_t acalendarid);

		void add_activity(int64_t actorid, std::shared_ptr<activity>& activ);

		bool timer_handle(const message<np_timerparm>& adata);
		bool handle(const message<np_arg_null>&);
		bool handle(const message<np_actor_activity>& adata);
	};
}//namespace ngl
