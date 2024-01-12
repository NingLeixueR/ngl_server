#pragma once

#include "actor_manage.h"
#include "actor_db_client.h"
#include "actor_protocol.h"
#include "actor_timer.h"
#include "actor_create.h"
#include "db_data.h"
#include "db.h"
#include "db_pool.h"
#include "db_manage.h"
#include "net.h"
#include "logprintf.h"

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

		static void actor_register();

		virtual ~actor_log();

		bool handle(message<actor_logitempro>& adata);
	};
}