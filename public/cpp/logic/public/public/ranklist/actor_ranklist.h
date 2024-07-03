#pragma once

#include "actor_manage.h"
#include "actor_create.h"
#include "ndb_modular.h"
#include "nsp_server.h"
#include "nsp_client.h"
#include "db_manage.h"
#include "ndbclient.h"
#include "nprotocol.h"
#include "ranklist.h"
#include "db_data.h"
#include "db_pool.h"
#include "ntimer.h"
#include "notice.h"
#include "net.pb.h"
#include "net.h"
#include "db.h"

namespace ngl
{
	class actor_ranklist : public actor
	{
		actor_ranklist(const actor_ranklist&) = delete;
		actor_ranklist& operator=(const actor_ranklist&) = delete;

		ranklist m_ranklist;

		//tdb_brief::nsp_cli<actor_ranklist>::getconst()

		actor_ranklist();
	public:
		friend class actor_instance<actor_ranklist>;
		static actor_ranklist& getInstance()
		{
			return actor_instance<actor_ranklist>::instance();
		}

		virtual void init()
		{
			//m_ranklist.set(this);
		}

		static void nregister();

		virtual ~actor_ranklist() {}

		virtual void loaddb_finish(bool adbishave) {}

		bool handle(message<mforward<pbnet::PROBUFF_NET_RANKLIST>>& adata);

		using handle_cmd = cmd<actor_ranklist, std::string, int, ngl::json_read&>;
		// # GM²Ù×÷
		bool handle(message<mforward<np_gm>>& adata);
	};
}// namespace ngl