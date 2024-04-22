#pragma once

#include "actor_manage.h"
#include "actor_create.h"
#include "ndb_modular.h"
#include "nroleitems.h"
#include "ndbclient.h"
#include "nprotocol.h"
#include "calendar.h"
#include "ntimer.h"
#include "nlog.h"
#include "net.h"
#include "db_manage.h"
#include "db_pool.h"
#include "db_data.h"
#include "db.h"

namespace ngl
{
	class actor_calendar : public actor
	{
		calendar m_calendar;

		actor_calendar();
	public:
		friend class actor_instance<actor_calendar>;
		static actor_calendar& getInstance()
		{
			return actor_instance<actor_calendar>::instance();
		}

		virtual void init()
		{
			m_calendar.set(this);
		}

		static void nregister();

		virtual ~actor_calendar() {}

		virtual void loaddb_finish(bool adbishave) {}

		static i64_actorid actorid()
		{
			return nguid::make(ACTOR_CALENDAR, ttab_servers::tab()->m_area, nguid::none_actordataid());
		}

		bool handle(message<np_calendar>& adata);
	};
}//namespace ngl