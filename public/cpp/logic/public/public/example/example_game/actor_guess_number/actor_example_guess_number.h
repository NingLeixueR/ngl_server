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
	class actor_example_guess_number : public actor
	{
		actor_example_guess_number(const actor_example_guess_number&) = delete;
		actor_example_guess_number& operator=(const actor_example_guess_number&) = delete;

		int32_t m_maxnumber;
		int32_t m_minnumber;
		int32_t m_index;
		int32_t m_setputc;
		int32_t m_bombvalues;

		std::vector<i64_actorid> m_rolesds;
	public:
		actor_example_guess_number(const std::set<i64_actorid>& aroleids, int32_t aindex);

		static ENUM_ACTOR actor_type()
		{
			return ACTOR_EXAMPLE_GUESS_NUMBER;
		}

		static i64_actorid actorid()
		{
			return nguid::make(ACTOR_EXAMPLE_GUESS_NUMBER, tab_self_area, nguid::none_actordataid());
		}

		virtual ~actor_example_guess_number() = default;

		static void nregister();

		virtual void loaddb_finish(bool adbishave) {}

		bool timer_handle(const message<timerparm>& adata);

		void next_setp();

		void bomb(i64_actorid aroleid);

		// # 所有人准备就绪 可以进入游戏
		bool handle(const message<np_example_entergame_ready>& adata);

		bool handle(const message<mforward<pbexample::PROBUFF_EXAMPLE_GUESS_NUMBER>>& adata);
	};

}//namespace ngl