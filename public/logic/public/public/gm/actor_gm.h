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
#include "actor_create.h"
#include "db_modular.h"
#include "notice.h"
#include "splite.h"
#include "manage_csv.h"
#include "gm.pb.h"
#include "init_protobuf.h"

namespace ngl
{
	class actor_gm : public actor
	{
		actor_gm();
	public:
		friend class actor_instance<actor_gm>;
		static actor_gm& getInstance()
		{
			return actor_instance<actor_gm>::instance();
		}

		virtual void init(){}

		static void actor_register();

		virtual ~actor_gm() {}

		virtual void loaddb_finish(bool adbishave) {}

		enum { ACTOR_TYPE = ACTOR_GM};


		template <typename T>
		void sendphpclient(mforward<T>& adata)
		{
			send(adata.identifier(), *adata.data(), actor_guid::make(), actor_guid::make());
		}

		template <typename T>
		void send2other(ENUM_ACTOR atype, const pack* apack, T& apro)
		{
			std::shared_ptr<mforward<T>> pro(new mforward<T>(apack->m_id, apro));
			send_actor(actor_guid::make_self(atype), pro);
		}

		template <typename T>
		void send2role(const pack* apack, T& apro)
		{
			std::shared_ptr<mforward<T>> pro(new mforward<T>(apack->m_id, apro));
			send_actor(apro.m_roleid(), pro);
		}

		template <ENUM_ACTOR MODULE,typename T>
		bool handle(message<T>& adata)
		{
			send2other(MODULE, adata.m_pack, *adata.m_data);
			return true;
		}

		// ·¢ËÍ¸øactor_role
		template <typename T>
		bool handle_role(message<T>& adata)
		{
			send2role(adata.m_pack, *adata.m_data);
			return true;
		}
		
		template <typename T>
		bool handle(message<mforward<T>>& adata)
		{
			sendphpclient(*adata.m_data);
			return true;
		}

		private:
	};
}