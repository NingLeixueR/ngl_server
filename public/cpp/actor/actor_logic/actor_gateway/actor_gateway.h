/*
* Copyright (c) [2020-2025] NingLeixueR
* 
* ��Ŀ���ƣ�ngl_server
* ��Ŀ��ַ��https://github.com/NingLeixueR/ngl_server
* 
* ���ļ��� ngl_server ��Ŀ��һ���֣���ѭ MIT ��ԴЭ�鷢����
* �����԰���Э��涨����ʹ�á��޸ĺͷַ�����Ŀ��������ҵ��;��
* ���豣��ԭʼ��Ȩ�����������
* 
* �������μ���Ŀ��Ŀ¼�µ� LICENSE �ļ���
* https://github.com/NingLeixueR/ngl_server/blob/main/LICENSE
*/
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
	class actor_gateway : 
		public actor
	{
		actor_gateway(const actor_gateway&) = delete;
		actor_gateway& operator=(const actor_gateway&) = delete;

		gateway_info m_info;

		actor_gateway();
	public:
		friend class actor_instance<actor_gateway>;
		static actor_gateway& instance() 
		{ 
			return actor_instance<actor_gateway>::instance(); 
		}

		virtual ~actor_gateway() = default;

		static ENUM_ACTOR actor_type();

		static i64_actorid actorid(i32_actordataid aactordataid);

		virtual void init();

		virtual void loaddb_finish(bool adbishave);

		static void nregister();

		// # ֪ͨactor_server [actorid]->[gateway server id]
		void sync_actorserver_gatewayid(const nguid& aguid, bool aisremove);

		void update_gateway_info(const std::shared_ptr<np_actor_gatewayinfo_updata>& ap);

		void session_close(gateway_socket* ainfo);

		bool timer_handle(const message<np_timerparm>& adata);

		bool handle(const message<np_arg_null>&);

		bool handle(const message<np_gateway_close_session>& adata);

		// # actor_login֪ͨgateway����Ѿ���¼(����¼session����Ӧ��game������)
		bool handle(const message<np_actorrole_login>& adata);

		// # ��ȡkcp-session
		bool handle(const message<np_actor_kcp>& adata);

		// # ����л�game����
		bool handle(const message<np_actorswitch_process<np_actorswitch_process_role>>& adata);

		// # �Ͽ�����
		bool handle(const message<np_actor_session_close>& adata);
		
		// # ��ҵ�½
		bool handle(const message<pbnet::PROBUFF_NET_ROLE_LOGIN>& adata);

		// # ��ȡkcp-session
		bool handle(const message<pbnet::PROBUFF_NET_KCPSESSION>& adata);
	};
}//namespace ngl

