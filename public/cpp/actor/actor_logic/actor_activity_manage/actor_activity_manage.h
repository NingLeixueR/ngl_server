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

		virtual void loaddb_finish(bool adbishave);

		void activity_start(int64_t aactivityid, int64_t atime, int32_t acalendarid);
		void activity_finish(int64_t aactivityid, int64_t atime, int32_t acalendarid);
	public:
		static ENUM_ACTOR actor_type()
		{
			return ACTOR_ACTIVITY_MANAGE;
		}

		static i64_actorid actorid()
		{
			return nguid::make(ACTOR_ACTIVITY_MANAGE, tab_self_area, nguid::none_actordataid());
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

		static void nregister();
		
		bool handle(const message<np_actor_activity>& adata);

		virtual void init();

		bool timer_handle(const message<np_timerparm>& adata);
	private:
	};
}//namespace ngl