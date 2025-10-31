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

#include "actor_gateway_c2g.h"
#include "actor_gateway_g2c.h"
#include "actor_gateway.h"
#include "template_arg.h"
#include "actor_robot.h"
#include "actor_role.h"
#include "actor.h"
#include "xml.h"

namespace ngl
{
	class nforward
	{
		template <typename ...ARG>
		static void register_c2g()
		{
			if (ngl::GATEWAY == xmlnode::m_nodetype)
			{
				actor_gateway_c2g::register_forward_c2g<actor_gateway_c2g>:: template func<ARG...>();
				return;
			}
			if (ngl::GAME == xmlnode::m_nodetype)
			{
				actor::register_handle<actor_role>:: template func <ARG...>(false);
				return;
			}
		}
		
		// ����ת��   [client]->[gateway]			->			[game]		->		[module]
		// ##############################		��һ��ת��		######	�ڶ���ת��	#######
		// ��Ҫ֧�ֶ���ת��
		template <ENUM_ACTOR ACTOR, typename ...ARG>
		static void register_c2g_2()
		{
			if (ngl::GATEWAY == xmlnode::m_nodetype)
			{
				actor_gateway_c2g::register_forward_c2g<actor_gateway_c2g>::template func<ARG...>();
				return;
			}
			if (ngl::GAME == xmlnode::m_nodetype)
			{
				actor::register_secondary_forward_c2g<actor_role, ACTOR>:: template func <ARG...>();
				return;
			}
		}

		template <typename ...ARG>
		static void register_g2c()
		{
			if (ngl::GATEWAY == xmlnode::m_nodetype)
			{
				actor_gateway_g2c::register_forward_g2c<actor_gateway_g2c>::template func<ARG...>();
				return;
			}
			if (ngl::ROBOT == xmlnode::m_nodetype)
			{
				actor::register_handle<actor_robot>:: template func <ARG...>(false);
				return;
			}
		}
	public:
		// # client->gateway->game
		static void c2g();

		// # game->gateway->client
		static void g2c();
	};
}//namespace ngl