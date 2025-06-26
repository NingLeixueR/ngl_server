#pragma once

#include "actor_manage.h"
#include "actor_create.h"
#include "ndb_modular.h"
#include "define_json.h"
#include "nsp_server.h"
#include "nsp_client.h"
#include "ndbclient.h"
#include "nprotocol.h"
#include "db_manage.h"
#include "db_data.h"
#include "db_pool.h"
#include "notice.h"
#include "ntimer.h"
#include "gcmd.h"
#include "net.h"
#include "db.h"

namespace ngl
{
	class actor_notice : 
		public actor
	{
		actor_notice(const actor_notice&) = delete;
		actor_notice& operator=(const actor_notice&) = delete;

		notice m_notice;

		actor_notice();
	public:
		friend class actor_instance<actor_notice>;
		static actor_notice& instance()
		{
			return actor_instance<actor_notice>::instance();
		}

		virtual ~actor_notice() = default;

		static ENUM_ACTOR actor_type();

		static i64_actorid actorid();

		virtual void init();

		virtual void loaddb_finish(bool adbishave);

		static void nregister();

		static bool sendnotice(const std::string& anotice, int32_t astarttime, int32_t afinishtime);

		using handle_cmd = cmd<actor_notice, std::string, int, const ngl::json_read&>;

		bool timer_handle(const message<np_timerparm>& adata);

		bool handle(const message<np_arg_null>&);
		bool handle(const message<mforward<np_gm>>& adata);
		// [请求]获取公告列表
		bool handle(const message<mforward<pbnet::PROBUFF_NET_NOTICE>>& adata);
		bool handle(const message<np_actor_addnotice>& adata);
	};
}//namespace ngl
