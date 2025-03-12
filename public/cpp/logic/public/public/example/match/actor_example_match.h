#pragma once

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
	class actor_example_match : public actor
	{
		actor_example_match(const actor_example_match&) = delete;
		actor_example_match& operator=(const actor_example_match&) = delete;

		actor_example_match() :
			actor(
				actorparm
				{
					.m_parm
						{
							.m_type = ACTOR_EXAMPLE_MATCH,
							.m_area = tab_self_area
						},
					.m_weight = 0x7fffffff,
				})
		{
		}

	public:
		friend class actor_instance<actor_example_match>;
		static actor_example_match& getInstance()
		{
			return actor_instance<actor_example_match>::instance();
		}

		static ENUM_ACTOR actor_type()
		{
			return ACTOR_EXAMPLE_MATCH;
		}

		static i64_actorid actorid()
		{
			return nguid::make(ACTOR_EXAMPLE_MATCH, tab_self_area, nguid::none_actordataid());
		}

		virtual ~actor_example_match() {}

		static void nregister()
		{
			// 定时器
			actor::register_timer<actor_example_match>(&actor_example_match::timer_handle);
			// 协议注册
			register_handle_proto<actor_example_match>::func<
				mforward<pbexample::PROBUFF_NET_EXAMPLE_PLAY_JOIN>,
				mforward<pbexample::PROBUFF_NET_EXAMPLE_PLAY_MATCHING>,
				mforward<pbexample::PROBUFF_NET_EXAMPLE_PLAY_PLAYER_CONFIRM>
			>(false);
		}

		virtual void loaddb_finish(bool adbishave) {}

		bool handle(const message<mforward<pbexample::PROBUFF_NET_EXAMPLE_PLAY_JOIN>>& adata)
		{
			return true;
		}

		bool handle(const message<mforward<pbexample::PROBUFF_NET_EXAMPLE_PLAY_MATCHING>>& adata)
		{
			return true;
		}

		bool handle(const message<mforward<pbexample::PROBUFF_NET_EXAMPLE_PLAY_PLAYER_CONFIRM>>& adata)
		{
			return true;
		}

		virtual void init()
		{
			timerparm tparm;
			if (make_timerparm::make_interval(tparm, 1) == false)
			{
				log_error()->print("actor_chat::init() make_timerparm::make_interval(tparm, 2) == false!!!");
				return;
			}
			set_timer(tparm);
		}

		bool timer_handle(const message<timerparm>& adata)
		{
			return true;
		}
	};
}//namespace ngl