#pragma once

#include "actor_protocol.h"
#include "actor_roleitem.h"
#include "actor_manage.h"
#include "actor_create.h"
#include "actor_timer.h"
#include "manage_csv.h"
#include "db_modular.h"
#include "db_manage.h"
#include "db_data.h"
#include "db_pool.h"
#include "db.h"
#include "ndbclient.h"
#include "briefdb.h"
#include "net.pb.h"
#include "net.h"

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

		static void nregister();

		virtual void init()
		{
			m_briefdb.set(this);
		}

		virtual ~actor_brief() {}

		virtual void loaddb_finish(bool adbishave) {}

		bool handle(message<actor_roleinfo>& adata)
		{
			
			std::cout << "#####:" << (int64_t)(adata.m_data->m_vecinfo.m_data.get()) << std::endl;
			
			m_briefdb.update(*adata.m_data->m_vecinfo.m_data.get());

			auto pro = std::make_shared<actor_roleinfo>();
			*pro = *adata.m_data;
			actor::static_send_actor(nguid::make_self(ACTOR_NOTICE), nguid::make(), pro);
			actor::static_send_actor(nguid::make_self(ACTOR_CHAT), nguid::make(), pro);
		
			actor::static_send_actor(
			nguid::make(ACTOR_CHAT,ttab_servers::tab()->m_crossarea, nguid::none_actordataid()),
			nguid::make(), pro);
			return true;
		}


		

	private:
	};
}