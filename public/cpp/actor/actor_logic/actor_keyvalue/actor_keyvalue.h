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

		void init() final;

		void loaddb_finish(bool adbishave) final;

		static ENUM_ACTOR actor_type();

		static i64_actorid actorid();

		static void nregister();
	};
}//namespace ngl