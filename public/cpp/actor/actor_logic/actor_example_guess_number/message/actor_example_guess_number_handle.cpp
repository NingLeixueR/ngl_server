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
#include "actor_example_guess_number.h"
#include "actor_brief.h"
namespace ngl
{
	bool actor_example_guess_number::handle(const message<mforward<pbexample::PROBUFF_EXAMPLE_GUESS_NUMBER>>& adata)
	{
		const pbexample::PROBUFF_EXAMPLE_GUESS_NUMBER* lpdata = adata.get_data()->data();
		i64_actorid roleid = adata.get_data()->identifier();

		if (isfinish())
		{//��Ϸ�Ѿ�����
			bomb();
			return true;
		}
		if (roleid != next_guess_role())
		{
			send_error(roleid, pbexample::E_GUESS_NUMBER_ERROR_NOT_GUESS);
			return true;
		}
		int32_t lnumber = lpdata->mguessnumber();
		if (lnumber >= m_maxnumber || lnumber <= m_minnumber)
		{
			send_error(roleid, pbexample::E_GUESS_NUMBER_ERROR_VALUE);
			return true;
		}

		if (lnumber == m_bombvalues)
		{//ը�� ��Ϸ����
			set_finish(roleid);
			bomb();
			return true;
		}

		if (lnumber > m_bombvalues)
		{
			m_maxnumber = lnumber;
		}
		else
		{
			m_minnumber = lnumber;
		}

		int32_t lsize = m_maxnumber - 1 - m_minnumber;
		if (lsize <= 1)
		{//��Ϸ���� ��һ���˱�ը
			set_finish(next_guess_role());
			bomb();
			return true;
		}

		next_setp();
		return true;
	}
	bool actor_example_guess_number::handle(const message<np_example_entergame_ready>& adata)
	{
		next_setp();
		return true;
	}
}//namespace ngl
