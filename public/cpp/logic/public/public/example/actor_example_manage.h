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
			example_waittime = 30,	// �ȴ�������Ϸ���������
		};

		std::array<int32_t, eenum::exampleindex_max> m_exampleindex;

		struct playinfo
		{
			std::map<i64_actorid, bool> m_role_enter_example; // ����Ƿ����������Ϸ
			std::set<i64_actorid> m_roles;
			i64_actorid m_actorexampleid = nguid::make();
			int32_t m_createexample = 0;
		};
		std::map<pbexample::EPLAY_TYPE, std::map<i64_actorid, playinfo>> m_info;
		std::map<pbexample::EPLAY_TYPE, std::map<i64_actorid, playinfo>> m_finishinfo;

		using type_tuple = std::tuple<pbexample::EPLAY_TYPE, i64_actorid>;		
		std::map<i64_actorid, type_tuple> m_playerexample; // key:roleid value:actor_exmple_xxx

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

		virtual ~actor_example_manage() = default;

		static void nregister()
		{
			// ��ʱ��
			actor::register_timer<actor_example_manage>(&actor_example_manage::timer_handle);

			// Э��ע��
			register_handle_proto<actor_example_manage>::func<
				mforward<pbexample::PROBUFF_EXAMPLE_PLAY_ENTER_EXAMPLE>
			>(false);

			register_handle_custom<actor_example_manage>::func<
				np_create_example
			>(true);
		}
		virtual void init();

		virtual void loaddb_finish(bool adbishave) {}

		void enter_game(playinfo* applayinfo, i64_actorid aroleid, pbexample::ECROSS across, pbexample::EPLAY_TYPE atype);

		// # ƥ��ɹ��� ���������淨
		bool handle(const message<np_create_example>& adata);

		// # ��ҵ�½
		bool handle(const message<np_login_request_info>& adata);

		// # ��ҽ���������Ϸ
		bool handle(const message<mforward<pbexample::PROBUFF_EXAMPLE_PLAY_ENTER_EXAMPLE>>& adata);
		
		bool timer_handle(const message<timerparm>& adata);
	};

}//namespace ngl