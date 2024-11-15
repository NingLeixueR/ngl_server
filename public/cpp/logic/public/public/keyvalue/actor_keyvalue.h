#pragma once

#include "actor_manage.h"
#include "actor_create.h"
#include "ndb_modular.h"
#include "nsp_server.h"
#include "manage_csv.h"
#include "keyvaluedb.h"
#include "nprotocol.h"
#include "db_manage.h"
#include "ndbclient.h"
#include "db_data.h"
#include "db_pool.h"
#include "ntimer.h"
#include "net.pb.h"
#include "net.h"
#include "db.h"

namespace ngl
{
	class actor_keyvalue : public actor
	{
		actor_keyvalue(const actor_keyvalue&) = delete;
		actor_keyvalue& operator=(const actor_keyvalue&) = delete;

		actor_keyvalue();

		keyvaluedb m_keyvaluedb;
	public:
		friend class actor_instance<actor_keyvalue>;
		static actor_keyvalue& getInstance()
		{
			return actor_instance<actor_keyvalue>::instance();
		}

		static void nregister();

		void init() final
		{
			m_keyvaluedb.set(this);
		}

		void loaddb_finish(bool adbishave) final 
		{
			log_error()->print("actor_keyvalue load finish!!!");
		}

		static ENUM_ACTOR actor_type()
		{
			return ACTOR_KEYVALUE;
		}

		static i64_actorid actorid()
		{
			return nguid::make(actor_type(), tab_self_area, nguid::none_actordataid());
		}
	};
}//namespace ngl