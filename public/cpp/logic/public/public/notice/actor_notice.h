#pragma once

#include "actor_manage.h"
#include "actor_create.h"
#include "actor_timer.h"
#include "db_manage.h"
#include "db_data.h"
#include "db_pool.h"
#include "db.h"
#include "ndb_modular.h"
#include "nroleitems.h"
#include "ndbclient.h"
#include "nprotocol.h"
#include "notice.h"
#include "net.h"

namespace ngl
{
	class actor_notice : public actor
	{
		notice m_notice;

		actor_notice();
	public:
		friend class actor_instance<actor_notice>;
		static actor_notice& getInstance()
		{
			return actor_instance<actor_notice>::instance();
		}

		virtual void init()
		{
			m_notice.set(this);
		}
		static void nregister();

		virtual ~actor_notice() {}

		virtual void loaddb_finish(bool adbishave) {}

		void get_notice_list(GM::PROBUFF_GM_GET_NOTICE_RESPONSE& anotice)
		{
			for (auto& [key, value] : m_notice.data())
			{
				GM::gm_notice* lptemp = anotice.add_m_notice();
				lptemp->set_m_id(value.getconst().m_id());
				lptemp->set_m_notice(value.getconst().m_notice());
				lptemp->set_m_starttime(value.getconst().m_starttime());
				lptemp->set_m_finishtime(value.getconst().m_finishtime());
			}
		}
		
		bool handle(message<mforward<GM::PROBUFF_GM_GET_NOTICE>>& adata)
		{
			using type = mforward<GM::PROBUFF_GM_GET_NOTICE_RESPONSE>;
			std::shared_ptr<type> pro(new type(adata.m_data->identifier()));
			get_notice_list(*pro->add_data());
			send_actor(nguid::make_self(ACTOR_GM), pro);
			return true;
		}
		
		bool handle(message<mforward<GM::PROBUFF_GM_ADD_NOTICE>>& adata)
		{
			GM::PROBUFF_GM_ADD_NOTICE* lptr = adata.m_data->data();
			m_notice.add_notice(lptr->m_notice().m_notice(), lptr->m_notice().m_starttime(), lptr->m_notice().m_finishtime());
			
			using type = mforward<GM::PROBUFF_GM_ADD_NOTICE_RESPONSE>;
			std::shared_ptr<type> pro(new type(adata.m_data->identifier()));
			pro->add_data()->set_m_stat(true);
			send_actor(nguid::make_self(ACTOR_GM), pro);
			return true;
		}
		
		bool handle(message<mforward<GM::PROBUFF_GM_DEL_NOTICE>>& adata)
		{
			m_notice.erase(adata.m_data->data()->m_id());
			using type = mforward<GM::PROBUFF_GM_DEL_NOTICE_RESPONSE>;
			std::shared_ptr<type> pro(new type(adata.m_data->identifier()));
			pro->add_data()->set_m_stat(true);
			send_actor(nguid::make_self(ACTOR_GM), pro);
			return true;
		}
		
		bool handle(message<mforward<pbnet::PROBUFF_NET_GET_NOTICE>>& adata)
		{
			auto pro = std::make_shared<pbnet::PROBUFF_NET_GET_NOTICE_RESPONSE>();
			for (const auto& [_id, _notice] : m_notice.data())
			{
				*pro->add_m_notices() = _notice.getconst();
			}
			send_client(adata.m_data->identifier(), pro);
			return true;			
		}

	private:
	};
}