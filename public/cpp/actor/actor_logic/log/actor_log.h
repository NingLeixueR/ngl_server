#pragma once

#include "actor_manage.h"
#include "actor_create.h"
#include "nprotocol.h"
#include "ndbclient.h"
#include "logprintf.h"
#include "db_manage.h"
#include "db_data.h"
#include "db_pool.h"
#include "ntimer.h"
#include "db.h"
#include "net.h"

#include <fstream>
#include <memory>

namespace ngl
{
	class actor_log : public actor
	{
		std::shared_ptr<logfile> m_log;
	public:
		actor_log(ELOG_TYPE atype);

		virtual void init();

		static void nregister();

		virtual ~actor_log();

		static i64_actorid actorid(ELOG_TYPE atype)
		{
			return nguid::make(ACTOR_LOG, ttab_servers::tab()->m_area, atype);
		}

		bool handle(message<np_actor_logitem>& adata);
	};
}//namespace ngl