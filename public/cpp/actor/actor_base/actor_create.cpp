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
#include "actor_create.h"

namespace ngl
{
	actor_create::actor_create() :
		actor(
			actorparm
			{
				.m_parm
				{
					.m_type = ACTOR_CREATE,
					.m_area = tab_self_area,
					.m_id = nconfig::m_tcount,
				},
				.m_weight = 0x7fffffff,
			})
	{
	}

	i64_actorid actor_create::actorid(i32_serverid aserverid)
	{
		return nguid::make(ACTOR_CREATE, tab_self_area, aserverid);
	}

	void actor_create::nregister()
	{
		register_handle<actor_create>::func<
			np_actorswitch_process<np_actorswitch_process_role>
		>(false);
	}
}//namespace ngl