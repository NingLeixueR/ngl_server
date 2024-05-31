#pragma once

#include "actor_manage.h"
#include "actor_create.h"
#include "manage_curl.h"
#include "manage_csv.h"
#include "ndb_modular.h"
#include "ndbclient.h"
#include "nprotocol.h"
#include "tprotocol.h"
#include "ntimer.h"
#include "notice.h"
#include "splite.h"
#include "net.h"
#include "db_manage.h"
#include "db_pool.h"
#include "db_data.h"
#include "db.h"

namespace ngl
{
	class actor_gm : public actor
	{
		actor_gm(const actor_gm&) = delete;
		actor_gm& operator=(const actor_gm&) = delete;

		actor_gm();
	public:
		friend class actor_instance<actor_gm>;
		static actor_gm& getInstance()
		{
			return actor_instance<actor_gm>::instance();
		}

		virtual void init(){}

		static void nregister();

		virtual ~actor_gm() {}

		virtual void loaddb_finish(bool adbishave) {}

		void sendsign(ENUM_ACTOR atype, const pack* apack, ngl::np_gm& apro)
		{
			auto pro = std::make_shared<mforward<ngl::np_gm>>(apack->m_id, apro);
			send_actor(nguid::make_self(atype), pro);
		}

		void sendnosign(i64_actorid aactorid, const pack* apack, ngl::np_gm& apro)
		{
			auto pro = std::make_shared<mforward<ngl::np_gm>>(apack->m_id, apro);
			send_actor(aactorid, pro);
		}

		bool ret_gm(const pack* apack, ngl::np_gm_response& adata)
		{
			send(apack->m_id, adata, nguid::make(), nguid::make());
			return true;
		}

		using handle_cmd = cmd<actor_gm, std::string, ngl::json_read&>;

		bool handle(message<ngl::np_gm>& adata);

		bool handle(message<mforward<ngl::np_gm_response>>& adata);
	};
}// namespace ngl