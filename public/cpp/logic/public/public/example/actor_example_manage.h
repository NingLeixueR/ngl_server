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
	class actor_example_manage : public actor
	{
		actor_example_manage(const actor_example_manage&) = delete;
		actor_example_manage& operator=(const actor_example_manage&) = delete;

		enum eenum
		{
			exampleindex_max = pbexample::EPLAY_TYPE_MAX + 1,
		};

		int32_t m_exampleindex[eenum::exampleindex_max];

		actor_example_manage();
	public:
		friend class actor_instance<actor_example_manage>;
		static actor_example_manage& getInstance()
		{
			return actor_instance<actor_example_manage>::instance();
		}

		static ENUM_ACTOR actor_type()
		{
			return ACTOR_EXAMPLE_MANAGE;
		}

		static i64_actorid actorid()
		{
			return nguid::make(ACTOR_EXAMPLE_MANAGE, tab_self_area, nguid::none_actordataid());
		}

		virtual ~actor_example_manage() {}

		static void nregister()
		{
			// 协议注册
		/*	register_handle_proto<actor_example_manage>::func<
				mforward<pbexample::PROBUFF_EXAMPLE_PLAY_JOIN>,
				mforward<pbexample::PROBUFF_EXAMPLE_PLAY_PLAYER_CONFIRM>
			>(false);*/

			register_handle_custom<actor_example_manage>::func<
				np_request_match_info
			>(true);
		}

		virtual void loaddb_finish(bool adbishave) {}

		// # 匹配成功后 创建例子玩法
		bool handle(const message<np_create_example>& adata);
	};

}//namespace ngl