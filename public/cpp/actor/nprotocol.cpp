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
#include "nprotocol.h"
#include "actor_log.h"

namespace ngl
{
	bool nactor_logitem::m_init = false;

	void nactor_logitem::send(std::shared_ptr<np_logitem> pro, bool aisnet)
	{
		if (aisnet)
		{
			// # ACTOR_TYPE # ELOG_TYPE
			actor::send_actor(actor_log::actorid(ttab_servers::instance().tab()->m_log), nguid::make(), pro);
		}
		else
		{
			actor::send_actor(actor_log::actorid(nconfig::m_tid), nguid::make(), pro);
		}
	}
}//namespace ngl