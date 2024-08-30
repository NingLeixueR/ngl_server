#pragma once

#include "actor_manage.h"
#include "actor_create.h"
#include "ndb_modular.h"
#include "nsp_server.h"
#include "nsp_client.h"
#include "ndbclient.h"
#include "nprotocol.h"
#include "db_manage.h"
#include "db_pool.h"
#include "db_data.h"
#include "ntimer.h"
#include "family.h"
#include "type.h"
#include "drop.h"
#include "nlog.h"
#include "net.h"
#include "db.h"

namespace ngl
{
	class actor_family : public actor
	{
		actor_family(const actor_family&) = delete;
		actor_family& operator=(const actor_family&) = delete;

		friend class familyer;
		friend class family;
		familyer m_familyer;
		family m_family;

		actor_family();
	public:
		friend class actor_instance<actor_family>;
		static actor_family& getInstance()
		{
			return actor_instance<actor_family>::instance();
		}

		virtual void init()
		{
			m_familyer.set(this);
			m_family.set(this);
		}

		static void nregister();

		virtual ~actor_family() {}

		virtual void loaddb_finish(bool adbishave) {}

		enum
		{
			actor_type = ACTOR_FAMILY
		};

		static i64_actorid actorid(i16_area area)
		{
			return nguid::make(ACTOR_FAMILY, area, nguid::none_actordataid());
		}

		// # 创建军团
		bool handle(message<mforward<pbnet::PROBUFF_NET_CREATE_FAMIL>>& adata);

		// # 申请/取消加入军团
		bool handle(message<mforward<pbnet::PROBUFF_NET_JOIN_FAMIL>>& adata);

		// # 军团长同意/不同意玩家加入军团
		bool handle(message<mforward<pbnet::PROBUFF_NET_RATIFY_JOIN_FAMIL>>& adata);

		// # 军团长转让军团
		bool handle(message<mforward<pbnet::PROBUFF_NET_CEDE_FAMIL>>& adata);

		// # 离开军团
		bool handle(message<mforward<pbnet::PROBUFF_NET_LEAVE_FAMIL>>& adata);

		// # 获取军团列表
		bool handle(message<mforward<pbnet::PROBUFF_NET_FAMIL_LIST>>& adata);

		// # 修改军团名称
		bool handle(message<mforward<pbnet::PROBUFF_NET_CHANGE_FAMILNAME>>& adata);

		// # 军团签到
		bool handle(message<mforward<pbnet::PROBUFF_NET_FAMILSIGN>>& adata);

		using handle_cmd = cmd<actor_mail, std::string, int, ngl::json_read&>;
		// # GM操作
		bool handle(message<mforward<np_gm>>& adata);
	};
}// namespace ngl