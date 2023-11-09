#pragma once

#include "actor_manage.h"
#include "net.h"
#include "db_data.h"
#include "db.h"
#include "db_pool.h"
#include "db_manage.h"
#include "actor_db_client.h"
#include "actor_protocol.h"
#include "actor_timer.h"
#include "actor_switchprocess.h"
#include "db_modular.h"
#include "actor_roleitem.h"
#include "tab_proto.h"
#include "net.pb.h"
#include "manage_csv.h"
#include "briefdb.h"

namespace ngl
{
	class actor_brief : public actor
	{
		actor_brief();

		briefdb m_briefdb;
	public:
		friend class actor_instance<actor_brief>;
		static actor_brief& getInstance()
		{
			return actor_instance<actor_brief>::instance();
		}

		static void actor_register();

		virtual void init()
		{
			m_briefdb.set(this);
		}

		virtual ~actor_brief() {}

		virtual void loaddb_finish(bool adbishave) {}

		enum { ACTOR_TYPE = ACTOR_BRIEF};


		bool handle(
			i32_threadid athread, 
			const std::shared_ptr<pack>& apack, 
			actor_roleinfo& adata)
		{
			m_briefdb.update(*adata.m_vecinfo.m_data.get());

			auto pro = std::make_shared<actor_roleinfo>();
			*pro = adata;
			actor::static_send_actor(actor_guid::make_self(ACTOR_NOTICE), actor_guid::make(), pro);
			actor::static_send_actor(actor_guid::make_self(ACTOR_CHAT), actor_guid::make(), pro);
		
			actor::static_send_actor(
			actor_guid::make(ACTOR_CHAT,ttab_servers::tab()->m_crossarea, actor_guid::none_actordataid()),
			actor_guid::make(), pro);
			return true;
		}


		

	private:
	};
}