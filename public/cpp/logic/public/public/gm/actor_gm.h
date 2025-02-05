#pragma once

#include "actor_manage.h"
#include "actor_create.h"
#include "manage_curl.h"
#include "ndb_modular.h"
#include "manage_csv.h"
#include "ndbclient.h"
#include "nprotocol.h"
#include "tprotocol.h"
#include "db_manage.h"
#include "db_pool.h"
#include "db_data.h"
#include "ntimer.h"
#include "notice.h"
#include "net.h"
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

		static ENUM_ACTOR actor_type()
		{
			return ACTOR_GM;
		}

		static i64_actorid actorid(i16_area area)
		{
			return nguid::make(actor_type(), area, nguid::none_actordataid());
		}

		// # 转发给单例
		void sendbytype(ENUM_ACTOR atype, const pack* apack, const ngl::np_gm& apro);

		// # 转发给特定actor
		void sendbyactorid(i64_actorid aactorid, const pack* apack, const ngl::np_gm& apro);

		// # 回复gm.php
		bool reply_php(const pack* apack, ngl::np_gm_response& adata)const;

		static bool checklocalbytype(NODE_TYPE atype);

		using handle_cmd = cmd<actor_gm, std::string, const ngl::json_read&, const message<ngl::np_gm>*>;
		bool handle(const message<ngl::np_gm>& adata);

		bool handle(const message<mforward<ngl::np_gm_response>>& adata);
	};
}// namespace ngl