#pragma once

#include "actor_manage.h"
#include "net.h"
#include "db_data.h"
#include "db.h"
#include "db_pool.h"
#include "db_manage.h"
#include "actor_db_client.h"
#include "actor_protocol.h"
#include "actor_timer.h"
#include "actor_create.h"
#include "db_modular.h"
#include "actor_roleitem.h"
#include "net.pb.h"
#include "manage_csv.h"
#include "activitydb.h"
#include "activity.h"

namespace ngl
{
	class actor_manage_activity : public actor
	{
		actor_manage_activity();

		struct roleitem
		{
			pbdb::db_brief m_info;
		};
		using type_roleitems = roleitems<actor_manage_activity, roleitem>;

		activitydb m_db;
		std::map<int64_t, activity*> m_allactivity;
	public:
		friend class actor_instance<actor_manage_activity>;
		static actor_manage_activity& getInstance()
		{
			return actor_instance<actor_manage_activity>::instance();
		}

		static void actor_register();

		virtual ~actor_manage_activity() {}

		virtual void loaddb_finish(bool adbishave);

	private:
		bool open_activity(i64_actorid m_activityid);
		bool close_activity(i64_actorid m_activityid);
		void init_activity(int64_t aactivityid);
		void start_activity(int64_t aactivityid);
	public:
		
		bool handle(message<actor_protocol_open_activity>& adata);
		bool handle(message<actor_protocol_close_activity>& adata);

		virtual void init();

		bool timer_handle(message<timerparm>& adata);
	private:
	};
}