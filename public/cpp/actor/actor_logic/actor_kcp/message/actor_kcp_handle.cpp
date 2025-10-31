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
#include "actor_kcp.h"
#include "ukcp.h"
namespace ngl
{
	bool actor_kcp::handle(const message<np_actor_kcp>& adata)
	{
		auto lpram = adata.get_data();
		auto lpack = adata.get_pack();
		nets::kcp()->reset_add(lpram->m_conv, lpram->m_uip, lpram->m_uport);

		nets::sendbysession(lpack->m_id, *lpram, nguid::make(), nguid::make());
		return true;
	}
}//namespace ngl
