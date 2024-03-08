#pragma once

#include "actor_manage.h"
#include "actor_protocol.h"
#include "actor_timer.h"
#include "actor_create.h"
#include "ndbclient.h"
#include "logprintf.h"
#include "db_manage.h"
#include "db_data.h"
#include "db_pool.h"
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

		bool handle(message<actor_logitempro>& adata);
	};
}