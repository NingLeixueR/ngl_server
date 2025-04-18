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

	class actor_log : public actor
	{
		actor_log() = delete;
		actor_log(const actor_log&) = delete;
		actor_log& operator=(const actor_log&) = delete;

		std::shared_ptr<logfile> m_log;
	public:
		actor_log(i32_actordataid aid);

		virtual ~actor_log() = default;

		static ENUM_ACTOR actor_type();

		static i64_actorid actorid(ENUM_ACTOR aactortype, ELOG_TYPE alogtype);

		virtual void init();

		virtual void loaddb_finish(bool adbishave);

		static void nregister();

		bool timer_handle(const message<np_timerparm>& adata);

		bool handle(const message<np_arg_null>&);

		bool handle(const message<np_logitem>& adata);
	};
}//namespace ngl