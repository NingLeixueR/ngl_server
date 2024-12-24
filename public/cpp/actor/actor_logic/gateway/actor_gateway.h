#pragma once

#include "gateway_info.h"
#include "actor_manage.h"
#include "actor_role.h"
#include "ndbclient.h"
#include "db_manage.h"
#include "db_pool.h"
#include "db_data.h"
#include "net.h"
#include "db.h"

namespace ngl
{
	class actor_gateway : public actor
	{
		actor_gateway(const actor_gateway&) = delete;
		actor_gateway& operator=(const actor_gateway&) = delete;

		actor_gateway();
	public:
		friend class actor_instance<actor_gateway>;
		static actor_gateway& getInstance() 
		{ 
			return actor_instance<actor_gateway>::instance(); 
		}

		virtual ~actor_gateway();

		static void nregister();

		static ENUM_ACTOR actor_type()
		{
			return ACTOR_GATEWAY;
		}

		static i64_actorid actorid(i32_actordataid aactordataid)
		{
			return nguid::make(actor_type(), tab_self_area, aactordataid);
		}

		gateway_info m_info;

		// # ֪ͨactor_server [actorid]->[gateway server id]
		void sync_actorserver_gatewayid(const nguid& aguid, bool aisremove);

		void update_gateway_info(const std::shared_ptr<np_actor_gatewayinfo_updata>& ap);

		bool handle(const message<np_gateway_close_session>& adata);
		void session_close(gateway_socket* ainfo);

		// # actor_login֪ͨgateway����Ѿ���¼(����¼session����Ӧ��game������)
		bool handle(const message<np_actorrole_login>& adata);

		// # ��ҵ�½
		bool handle(const message<pbnet::PROBUFF_NET_ROLE_LOGIN>& adata);

		// # ��ȡkcp-session
		bool handle(const message<np_actor_kcp>& adata);

		bool handle(const message<pbnet::PROBUFF_NET_KCPSESSION>& adata);

		// # ����л�game����
		bool handle(const message<np_actorswitch_process<np_actorswitch_process_role>>& adata);

		// # �Ͽ�����
		bool handle(const message<np_actor_session_close>& adata);
	};
}//namespace ngl

