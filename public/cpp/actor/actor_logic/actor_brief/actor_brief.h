#pragma once

#include "actor_manage.h"
#include "actor_create.h"
#include "ndb_modular.h"
#include "nsp_server.h"
#include "manage_csv.h"
#include "nprotocol.h"
#include "db_manage.h"
#include "ndbclient.h"
#include "db_data.h"
#include "db_pool.h"
#include "briefdb.h"
#include "ntimer.h"
#include "net.pb.h"
#include "net.h"
#include "db.h"

namespace ngl
{
	class actor_brief : public actor
	{
		actor_brief(const actor_brief&) = delete;
		actor_brief& operator=(const actor_brief&) = delete;

		actor_brief();

		briefdb m_briefdb;
	public:
		friend class actor_instance<actor_brief>;
		static actor_brief& getInstance()
		{
			return actor_instance<actor_brief>::instance();
		}

		virtual ~actor_brief() = default;

		static ENUM_ACTOR actor_type();

		static i64_actorid actorid();

		virtual void init();

		virtual void loaddb_finish(bool adbishave);

		static void nregister();

		bool timer_handle(const message<np_timerparm>& adata);
		bool handle(const message<np_arg_null>& );
	};
}//namespace ngl