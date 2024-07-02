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
			m_family.set(this);
		}

		static void nregister();

		virtual ~actor_family() {}

		virtual void loaddb_finish(bool adbishave) {}

		static i64_actorid actorid(i16_area area)
		{
			return nguid::make(ACTOR_FAMILY, area, nguid::none_actordataid());
		}

		// # 创建军团
		bool handle(message<mforward<pbnet::PROBUFF_NET_CREATE_FAMIL>>& adata)
		{
			i64_actorid lroleid = adata.get_data()->identifier();
			int32_t lstat = m_family.create_family(lroleid, adata.get_data()->data()->m_name());
			auto pro = std::shared_ptr<pbnet::PROBUFF_NET_CREATE_FAMIL_RESPONSE>();
			pro->set_m_stat(lstat);
			send_client(lroleid, pro);
			return true;
		}

		// # 加入军团
		bool handle(message<mforward<pbnet::PROBUFF_NET_JOIN_FAMIL>>& adata)
		{
			i64_actorid lroleid = adata.get_data()->identifier();
			int32_t lstat = m_family.join_family(lroleid, adata.get_data()->data()->m_familid());
			auto pro = std::shared_ptr<pbnet::PROBUFF_NET_JOIN_FAMIL_RESPONSE>();
			pro->set_m_stat(lstat);
			send_client(lroleid, pro);
			return true;
		}

		// # 离开军团
		bool handle(message<mforward<pbnet::PROBUFF_NET_LEAVE_FAMIL>>& adata)
		{
			i64_actorid lroleid = adata.get_data()->identifier();
			int32_t lstat = m_family.leave_family(lroleid, adata.get_data()->data()->m_familid());
			auto pro = std::shared_ptr<pbnet::PROBUFF_NET_LEAVE_FAMIL_RESPONSE>();
			pro->set_m_stat(lstat);
			send_client(lroleid, pro);
			return true;
		}

		// # 获取军团列表
		bool handle(message<mforward<pbnet::PROBUFF_NET_FAMIL_LIST>>& adata)
		{
			m_family.sync_family(adata.get_data()->identifier(), adata.get_data()->data()->m_familid());
			return true;
		}

		// # 修改军团名称
		bool handle(message<mforward<pbnet::PROBUFF_NET_CHANGE_FAMILNAME>>& adata)
		{
			i64_actorid lroleid = adata.get_data()->identifier();
			i64_actorid familid = adata.get_data()->data()->m_familid();
			int32_t lstat = m_family.change_familyname(lroleid, familid, adata.get_data()->data()->m_name());
			auto pro = std::shared_ptr<pbnet::PROBUFF_NET_CHANGE_FAMILNAME_RESPONSE>();
			pro->set_m_stat(lstat);
			send_client(lroleid, pro);
			return true;
		}

		using handle_cmd = cmd<actor_mail, std::string, int, ngl::json_read&>;
		// ## GM操作mail
		bool handle(message<mforward<np_gm>>& adata);

	};
}// namespace ngl