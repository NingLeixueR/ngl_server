#pragma once

#include "ttab_specialid.h"
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
#include "net.h"
#include "db.h"

#include "example.pb.h"
#include "net.pb.h"

namespace ngl
{
	class actor_guess_number : public actor
	{
		actor_guess_number(const actor_guess_number&) = delete;
		actor_guess_number& operator=(const actor_guess_number&) = delete;

		std::set<i64_actorid> m_rolesds;
		actor_guess_number(const std::set<i64_actorid>& aroleids, int32_t aindex);
	public:
		static ENUM_ACTOR actor_type()
		{
			return ACTOR_EXAMPLE_GUESS_NUMBER;
		}

		static i64_actorid actorid()
		{
			return nguid::make(ACTOR_EXAMPLE_GUESS_NUMBER, tab_self_area, nguid::none_actordataid());
		}

		virtual ~actor_guess_number() {}

		static void nregister()
		{
			// 协议注册
		/*	register_handle_proto<actor_guess_number>::func<
				mforward<pbexample::PROBUFF_EXAMPLE_PLAY_JOIN>,
				mforward<pbexample::PROBUFF_EXAMPLE_PLAY_PLAYER_CONFIRM>
			>(false);*/

			//register_handle_custom<actor_guess_number>::func<
			//	np_request_match_info
			//>(true);
		}

		virtual void loaddb_finish(bool adbishave) {}

		// # 创建好例子后进入游戏
		//bool handle(const message<np_create_example>& adata);
	};

}//namespace ngl