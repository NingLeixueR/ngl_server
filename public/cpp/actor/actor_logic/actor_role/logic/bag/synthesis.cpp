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
#include "actor_role.h"
#include "synthesis.h"
#include "consume.h"
#include "drop.h"

namespace ngl
{
	bool synthesis::use(actor_role* arole, int32_t aid)
	{
		tab_synthesis* tab = allcsv::tab<tab_synthesis>(aid);
		if (tab == nullptr)
		{
			return false;
		}
		if (consume::use(arole, tab->m_consume, 1) == false)
		{
			log_error()->print(
				"synthesis::use id:{} role:{} consume::use faile {}"
				, aid
				, arole->id_guid()
				, tab->m_consume
			);
			return false;
		}
		std::string lsrc = std::format("synthesis::use({},{})", nguid(arole->id_guid()), aid);
		if (arole->get_drop().use(tab->m_generate, 1, arole->id_guid(), lsrc, nullptr) == false)
		{
			log_error()->print(
				"synthesis::use id:{} role:{} drop::use faile {}"
				, aid
				, arole->id_guid()
				, tab->m_generate
			);
			return false;
		}
		return true;
	}
}// namespace ngl