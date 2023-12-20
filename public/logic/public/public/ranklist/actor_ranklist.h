#pragma once

#include "actor_manage.h"
#include "net.h"
#include "db_data.h"
#include "db.h"
#include "db_pool.h"
#include "db_manage.h"
#include "actor_db_client.h"
#include "actor_protocol.h"
//#include "actor_protocol_gm.h"
#include "actor_timer.h"
#include "actor_create.h"

#include "db_modular.h"
#include "notice.h"
#include "actor_roleitem.h"
#include "ranklist.h"

namespace ngl
{
	class actor_ranklist : public actor
	{
		ranklist m_ranklist;

		using type_roleitems = roleitems<actor_ranklist, pbdb::db_brief>;
		actor_ranklist();
	public:
		friend class actor_instance<actor_ranklist>;
		static actor_ranklist& getInstance()
		{
			return actor_instance<actor_ranklist>::instance();
		}

		virtual void init()
		{
			m_ranklist.set(this);
		}
		static void actor_register()
		{
		}

		virtual ~actor_ranklist() {}

		virtual void loaddb_finish(bool adbishave) {}

		/*bool handle(i32_threadid athread, const std::shared_ptr<pack>& apack, mforward<GM::PROBUFF_GM_GET_NOTICE>& adata)
		{
			using type = mforward<GM::PROBUFF_GM_GET_NOTICE_RESPONSE>;
			std::shared_ptr<type> pro(new type(adata.identifier()));
			get_notice_list(*pro->add_data());
			send_actor(actor_guid::make_self(ACTOR_GM), pro);
			return true;
		}*/

	private:
	};
}