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
#include "net.h"
#include "db.h"

#include <fstream>
#include <memory>

namespace ngl
{
	class actor_log : public actor
	{
		actor_log() = delete;
		actor_log(const actor_log&) = delete;
		actor_log& operator=(const actor_log&) = delete;

		std::shared_ptr<logfile> m_log;
	public:
		actor_log(i32_actordataid aid);

		virtual ~actor_log()=default;

		virtual void init();

		static void nregister();

		static ENUM_ACTOR actor_type();

		static i64_actorid actorid(ENUM_ACTOR aactortype, ELOG_TYPE alogtype);

		struct log_timerparm
		{
			enum etype
			{
				e_logflush,
				e_create,
			};
			etype m_type;
			log_timerparm(etype atype) :
				m_type(atype)
			{}
		};

		bool timer_handle(const message<np_timerparm>& adata);

		bool handle(const message<np_logitem>& adata);
	};
}//namespace ngl