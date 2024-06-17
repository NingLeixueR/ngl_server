#pragma once

#include "actor_manage.h"
#include "actor_create.h"
#include "manage_csv.h"
#include "db_manage.h"
#include "db_data.h"
#include "db_pool.h"
#include "db.h"
#include "ndb_modular.h"
#include "nroleitems.h"
#include "nprotocol.h"
#include "ndbclient.h"
#include "briefdb.h"
#include "ntimer.h"
#include "net.pb.h"
#include "net.h"

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

		virtual void loaddb_finish(bool adbishave) {}

		bool handle(message<np_actor_roleinfo>& adata);
	};
}//namespace ngl