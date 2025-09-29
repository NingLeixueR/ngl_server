#pragma once

#include "actor_manage.h"
#include "actor_create.h"
#include "ndb_modular.h"
#include "nsp_server.h"
#include "nsp_write.h"
#include "nsp_read.h"
#include "ndbclient.h"
#include "nprotocol.h"
#include "db_manage.h"
#include "testlua.h"
#include "db_pool.h"
#include "db_data.h"
#include "ntimer.h"
#include "drop.h"
#include "nlog.h"
#include "net.h"
#include "db.h"

namespace ngl
{
	class actor_testlua :
		public actor
	{
		actor_testlua(const actor_testlua&) = delete;
		actor_testlua& operator=(const actor_testlua&) = delete;

		testlua m_testlua;

		actor_testlua();
	public:
		friend class actor_instance<actor_testlua>;
		static actor_testlua& instance()
		{
			return actor_instance<actor_testlua>::instance();
		}
		virtual ~actor_testlua() = default;

		static ENUM_ACTOR actor_type();

		static i64_actorid actorid();

		virtual void init();

		virtual void erase_actor_before();

		virtual void loaddb_finish(bool adbishave);

		static void nregister();

		bool timer_handle(const message<np_timerparm>& adata);

		bool handle(const message<np_arg_null>&);
		// [ÇëÇó]É¾³ýºÃÓÑ
		bool handle(const message<mforward<pbnet::PROBUFF_NET_TESTLUA>>& adata);
	};
}// namespace ngl
