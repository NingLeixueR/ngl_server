#pragma once

#include "actor_manage.h"
#include "actor_create.h"
#include "manage_curl.h"
#include "ndb_modular.h"
#include "manage_csv.h"
#include "ndbclient.h"
#include "nprotocol.h"
#include "tprotocol.h"
#include "db_manage.h"
#include "db_pool.h"
#include "db_data.h"
#include "ntimer.h"
#include "notice.h"
#include "net.h"
#include "db.h"

namespace ngl
{
	class actor_gmclient : public actor
	{
		actor_gmclient(const actor_gmclient&) = delete;
		actor_gmclient& operator=(const actor_gmclient&) = delete;

		actor_gmclient();
	public:
		friend class actor_instance<actor_gmclient>;
		static actor_gmclient& getInstance()
		{
			return actor_instance<actor_gmclient>::instance();
		}

		virtual void init() {}

		static void nregister();

		virtual ~actor_gmclient() {}

		virtual void loaddb_finish(bool adbishave) {}

		static ENUM_ACTOR actor_type()
		{
			return ACTOR_GMCLIENT;
		}

		static i64_actorid actorid(i16_area area)
		{
			return nguid::make(ACTOR_GMCLIENT, area, ttab_servers::tab()->m_id);
		}

		struct protocols
		{
			std::map<int, std::string> m_promap;
			std::map<int, std::string> m_custommap;

			jsonfunc("proto", m_promap, "custom", m_custommap)
		};

		static void get_allprotocol(protocols& apro);

		// # GM²Ù×÷
		using handle_cmd = cmd<actor_gmclient, std::string, int, const ngl::json_read&>;
		bool handle(const message<mforward<np_gm>>& adata);
	};
}// namespace ngl