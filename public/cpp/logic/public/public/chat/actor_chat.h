#pragma once

#include "actor_manage.h"
#include "actor_create.h"
#include "ndb_modular.h"
#include "nroleitems.h"
#include "manage_csv.h"
#include "ndbclient.h"
#include "nprotocol.h"
#include "ntimer.h"
#include "net.pb.h"
#include "net.h"
#include "db_manage.h"
#include "db_data.h"
#include "db_pool.h"
#include "db.h"

namespace ngl
{
	class actor_chat : public actor
	{
		actor_chat(const actor_chat&) = delete;
		actor_chat& operator=(const actor_chat&) = delete;

		actor_chat();

		struct roleitem
		{
			pbdb::db_brief m_info;
			int32_t m_lastspeakutc = 0;
		};
		using type_roleitems = nroleitems<actor_chat, roleitem>;

		std::map<int, std::list<pbnet::chatitem>> m_chatitem;
		std::map<int, std::list<pbnet::chatitem>> m_update_chatitem;
	public:
		friend class actor_instance<actor_chat>;
		static actor_chat& getInstance()
		{
			return actor_instance<actor_chat>::instance();
		}

		static void nregister();

		virtual ~actor_chat() {}

		virtual void loaddb_finish(bool adbishave) {}

		bool handle(message<mforward<pbnet::PROBUFF_NET_CHAT>>& adata);

		virtual void init();

		bool timer_handle(message<timerparm>& adata);
	};
}//namespace ngl