#pragma once

#include "actor_manage.h"
#include "actor_create.h"
#include "db_manage.h"
#include "db_data.h"
#include "db_pool.h"
#include "db.h"
#include "ndb_modular.h"
#include "define_json.h"
#include "conversion.h"
#include "nroleitems.h"
#include "ndbclient.h"
#include "nprotocol.h"
#include "notice.h"
#include "ntimer.h"
#include "gcmd.h"
#include "net.h"

namespace ngl
{
	class actor_notice : public actor
	{
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

		struct tar {};
		using handle_cmd = cmd<tar, int, ngl::ojson&>;
		bool handle(message<mforward<np_gm>>& adata);

		//# 客户端拉取公告
		bool handle(message<mforward<pbnet::PROBUFF_NET_NOTICE>>& adata);		
	};
}