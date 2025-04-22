#pragma once

#include "actor_manage.h"
#include "actor_create.h"
#include "ndb_modular.h"
#include "nsp_server.h"
#include "nsp_client.h"
#include "manage_csv.h"
#include "ndbclient.h"
#include "nprotocol.h"
#include "db_manage.h"
#include "db_data.h"
#include "db_pool.h"
#include "ntimer.h"
#include "net.pb.h"
#include "net.h"
#include "db.h"

namespace ngl
{
	class actor_chat : public actor
	{
		actor_chat(const actor_chat&) = delete;
		actor_chat& operator=(const actor_chat&) = delete;

		std::map<int64_t, int>						m_lastspeakutc;
		std::map<int, std::list<pbnet::chatitem>>	m_chatitem;
		std::map<int, std::list<pbnet::chatitem>>	m_update_chatitem;

		actor_chat();
	public:
		friend class actor_instance<actor_chat>;
		static actor_chat& getInstance()
		{
			return actor_instance<actor_chat>::instance();
		}

		virtual ~actor_chat() = default;

		static ENUM_ACTOR actor_type();

		static i64_actorid actorid();

		virtual void init();

		virtual void loaddb_finish(bool adbishave);

		static void nregister();

		bool timer_handle(const message<np_timerparm>& adata);

		bool handle(const message<np_arg_null>&);
		// [«Î«Û]¡ƒÃÏ
		bool handle(const message<mforward<pbnet::PROBUFF_NET_CHAT>>& adata);
	};
}//namespace ngl
