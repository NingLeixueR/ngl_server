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
		std::map<int64_t, std::shared_ptr<activity>> m_allactivity;
	public:
		friend class actor_instance<actor_manage_activity>;
		static actor_manage_activity& getInstance()
		{
			return actor_instance<actor_manage_activity>::instance();
		}

		static void nregister();

		virtual ~actor_manage_activity() {}

		virtual void loaddb_finish(bool adbishave);

		void activity_start(int64_t aactivityid, int64_t atime, int32_t acalendarid);
		void activity_finish(int64_t aactivityid, int64_t atime, int32_t acalendarid);
	public:

		static i64_actorid actorid()
		{
			return nguid::make(ACTOR_ACTIVITY_MANAGE, ttab_servers::tab()->m_area, nguid::none_actordataid());
		}

		void add_activity(int64_t actorid, std::shared_ptr<activity>& activ)
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
		
		bool handle(message<np_actor_activity>& adata);

		virtual void init();

		bool timer_handle(message<timerparm>& adata);
	private:
	};
}