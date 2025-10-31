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

#include "actor_manage.h"
#include "naddress.h"
#include "net.h"

namespace ngl
{
	class actor_server : 
		public actor
	{
		actor_server(const actor_server&) = delete;
		actor_server& operator=(const actor_server&) = delete;

		actor_server();
	public:
		friend class actor_instance<actor_server>;
		static actor_server& instance()
		{
			return actor_instance<actor_server>::instance();
		}

		virtual ~actor_server() = default;

		static void nregister();

		static i64_actorid actorid();

		void print(const std::vector<i64_actorid>& avec)const;

		// # �ظ�/ת��[np_actornode_register]����
		void reply_np_actornode_register(const pack* apack, i32_serverid aserverid);
		void forward_np_actornode_register(
			const pack* apack, const nactornode& anode, i32_serverid aserverid, const std::vector<i64_actorid>& aadd
		);

		// # [actor client -> actor server] register
		// # ע����
		bool handle(const message<np_actornode_register>& adata);

		// # ���½���е�actor
		bool handle(const message<np_actornode_update_server>& adata);

		//# ��actor�ͻ���ͬ�������Ϣ(Ⱥ��)
		bool handle(const message<np_actornode_update_mass>& adata);

		// # ����gateway�� actor_role.guidid��gateway server id��Ӧ��ϵ
		bool handle(const message<np_actor_gatewayid_updata>& adata);
	};
}//namespace ngl