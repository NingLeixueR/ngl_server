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

		static void nregister();

		virtual void init()
		{
			m_briefdb.set(this);
		}

		virtual ~actor_brief() {}

		virtual void loaddb_finish(bool adbishave) 
		{
			log_error()->print("actor_brief::loaddb_finish({})", adbishave?"true":"false");
		}

		static ENUM_ACTOR actor_type()
		{
			return ACTOR_BRIEF;
		}

		static i64_actorid actorid()
		{
			return nguid::make(ACTOR_BRIEF, tab_self_area, nguid::none_actordataid());
		}
	};
}//namespace ngl