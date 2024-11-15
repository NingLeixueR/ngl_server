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
	class actor_notice : public actor
	{
		actor_notice(const actor_notice&) = delete;
		actor_notice& operator=(const actor_notice&) = delete;

		notice m_notice;

		actor_notice();
	public:
		friend class actor_instance<actor_notice>;
		static actor_notice& getInstance()
		{
			return actor_instance<actor_notice>::instance();
		}

		virtual void init()
		{
			m_notice.set(this);
		}

		static void nregister();

		virtual ~actor_notice() {}

		virtual void loaddb_finish(bool adbishave) {}

		static ENUM_ACTOR actor_type()
		{
			return ACTOR_NOTICE;
		}

		static i64_actorid actorid()
		{
			return nguid::make(ACTOR_NOTICE, tab_self_area, nguid::none_actordataid());
		}

		static bool sendnotice(const std::string& anotice, int32_t astarttime, int32_t afinishtime)
		{
			auto pro = std::make_shared<np_actor_addnotice>();
			pro->m_notice = anotice;
			pro->m_starttime = astarttime;
			pro->m_finishtime = afinishtime;
			actor::static_send_actor(actorid(), nguid::make(), pro);
			return true;
		}

		bool handle(const message<np_actor_addnotice>& adata);

		using handle_cmd = cmd<actor_notice, std::string, int, const ngl::json_read&>;
		bool handle(const message<mforward<np_gm>>& adata);

		//# �ͻ�����ȡ����
		bool handle(const message<mforward<pbnet::PROBUFF_NET_NOTICE>>& adata);
	};
}//namespace ngl