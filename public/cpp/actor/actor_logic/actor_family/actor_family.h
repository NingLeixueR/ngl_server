#pragma once

#include "events_logic.h"
#include "actor_manage.h"
#include "actor_create.h"
#include "actor_events.h"
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
	class actor_family : 
		public actor
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

		virtual ~actor_family() = default;

		static ENUM_ACTOR actor_type();

		static i64_actorid actorid();

		virtual void init();

		virtual void erase_actor_before();

		virtual void loaddb_finish(bool adbishave);

		static void nregister();
		
		// # GM操作
		using handle_cmd = cmd<actor_mail, std::string, int, const ngl::json_read&>;


		bool timer_handle(const message<np_timerparm>& adata);

		bool handle(const message<np_arg_null>&);
		bool handle(const message<mforward<np_gm>>& adata);
		// [请求]军团长转让军团
		bool handle(const message<mforward<pbnet::PROBUFF_NET_FAMIL_CEDE>>& adata);
		// [请求]修改军团名称
		bool handle(const message<mforward<pbnet::PROBUFF_NET_FAMIL_CHANGENAME>>& adata);
		// [请求]创建军团
		bool handle(const message<mforward<pbnet::PROBUFF_NET_FAMIL_CREATE>>& adata);
		// [请求]获取军团信息
		bool handle(const message<mforward<pbnet::PROBUFF_NET_FAMIL_INFO>>& adata);
		// [请求]申请/取消加入军团
		bool handle(const message<mforward<pbnet::PROBUFF_NET_FAMIL_JOIN>>& adata);
		// [请求]离开军团
		bool handle(const message<mforward<pbnet::PROBUFF_NET_FAMIL_LEAVE>>& adata);
		// [请求]获取军团列表
		bool handle(const message<mforward<pbnet::PROBUFF_NET_FAMIL_LIST>>& adata);
		// [请求]军团长同意/不同意玩家加入军团
		bool handle(const message<mforward<pbnet::PROBUFF_NET_FAMIL_RATIFY_JOIN>>& adata);
		// [请求]军团签到
		bool handle(const message<mforward<pbnet::PROBUFF_NET_FAMIL_SIGN>>& adata);
		bool handle(const message<np_eevents_logic_rolelogin>& adata);
		bool handle(const message<np_eevents_logic_roleoffline>& adata);
	};
}// namespace ngl
