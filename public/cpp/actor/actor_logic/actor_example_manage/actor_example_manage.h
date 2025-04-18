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
			example_waittime = 30,	// 等待进入游戏的最大秒数
		};

		std::array<int32_t, eenum::exampleindex_max> m_exampleindex;

		struct playinfo
		{
			i64_actorid m_actorexampleid = nguid::make();
			pbexample::EPLAY_TYPE m_type = pbexample::EPLAY_NULL;
			int32_t m_createexample = 0;
			std::map<i64_actorid, bool> m_role_enter_example; // 玩家是否进入例子游戏
			std::set<i64_actorid> m_roles;
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

		virtual ~actor_example_manage() = default;

		static ENUM_ACTOR actor_type();

		static i64_actorid actorid();

		virtual void init();

		virtual void loaddb_finish(bool adbishave);

		static void nregister();

		void enter_game(playinfo* applayinfo, i64_actorid aroleid, pbexample::ECROSS across, pbexample::EPLAY_TYPE atype);

		bool timer_handle(const message<np_timerparm>& adata);

		bool handle(const message<np_arg_null>&);

		// # 匹配成功后 创建例子玩法
		bool handle(const message<np_create_example>& adata);

		// # 玩家登陆
		bool handle(const message<np_login_request_info>& adata);

		// # 玩家进入例子游戏
		bool handle(const message<mforward<pbexample::PROBUFF_EXAMPLE_PLAY_ENTER_EXAMPLE>>& adata);

		// # 例子游戏销毁退出
		bool handle(const message<np_example_equit>& adata);
		
	};

}//namespace ngl