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
	class actor_gm : 
		public actor
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

		virtual ~actor_gm() = default;

		static ENUM_ACTOR actor_type();

		static i64_actorid actorid(i16_area area);

		virtual void init();

		virtual void loaddb_finish(bool adbishave);

		static void nregister();

		// # 转发给单例
		void sendbytype(ENUM_ACTOR atype, const pack* apack, const ngl::np_gm& apro);

		// # 转发给特定actor
		void sendbyactorid(i64_actorid aactorid, const pack* apack, const ngl::np_gm& apro);

		static bool checklocalbytype(NODE_TYPE atype);

		void init_handle_cmd();
		
		static bool sendtophp(i32_sessionid aidentifier, const ngl::np_gm_response& adata);

		using handle_cmd = cmd<actor_gm, std::string, const ngl::json_read&, const message<ngl::np_gm>*>;

		bool timer_handle(const message<np_timerparm>& adata);

		bool handle(const message<np_arg_null>&);
		bool handle(const message<mforward<ngl::np_gm_response>>& adata);
		bool handle(const message<ngl::np_gm>& adata);
	};
}// namespace ngl
