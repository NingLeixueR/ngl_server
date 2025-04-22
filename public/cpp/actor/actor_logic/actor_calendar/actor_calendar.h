#pragma once

#include "actor_manage.h"
#include "actor_create.h"
#include "ndb_modular.h"
#include "nsp_server.h"
#include "nsp_client.h"
#include "nprotocol.h"
#include "ndbclient.h"
#include "db_manage.h"
#include "calendar.h"
#include "db_pool.h"
#include "db_data.h"
#include "ntimer.h"
#include "nlog.h"
#include "net.h"
#include "db.h"

namespace ngl
{
	class actor_calendar : public actor
	{
		calendar m_calendar;

		actor_calendar(const actor_calendar&) = delete;
		actor_calendar& operator=(const actor_calendar&) = delete;

		actor_calendar();
	public:
		friend class actor_instance<actor_calendar>;
		static actor_calendar& getInstance()
		{
			return actor_instance<actor_calendar>::instance();
		}

		virtual ~actor_calendar() = default;

		static ENUM_ACTOR actor_type();

		static i64_actorid actorid();

		virtual void init();

		virtual void loaddb_finish(bool adbishave);

		static void nregister();

		bool timer_handle(const message<np_timerparm>& adata);

		bool handle(const message<np_arg_null>&);
		bool handle(const message<np_calendar>& adata);
	};
}//namespace ngl
