#pragma once

#include "actor_manage.h"
#include "actor_create.h"
#include "db_manage.h"
#include "db_data.h"
#include "db_pool.h"
#include "db.h"
#include "ndb_modular.h"
#include "nroleitems.h"
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
	class actor_manage_activity : public actor
	{
		actor_manage_activity();

		struct roleitem
		{
			pbdb::db_brief m_info;
		};
		using type_roleitems = nroleitems<actor_manage_activity, roleitem>;

		activitydb m_db;
		std::map<int64_t, activity*> m_allactivity;
	public:
		friend class actor_instance<actor_manage_activity>;
		static actor_manage_activity& getInstance()
		{
			return actor_instance<actor_manage_activity>::instance();
		}

		static void nregister();

		virtual ~actor_manage_activity() {}

		virtual void loaddb_finish(bool adbishave);

	private:
		bool open_activity(i64_actorid m_activityid);
		bool close_activity(i64_actorid m_activityid);
		void init_activity(int64_t aactivityid);
		void start_activity(int64_t aactivityid);
	public:
		
		bool handle(message<np_actor_openactivity>& adata);
		bool handle(message<np_actor_closeactivity>& adata);

		virtual void init();

		bool timer_handle(message<timerparm>& adata);
	private:
	};
}