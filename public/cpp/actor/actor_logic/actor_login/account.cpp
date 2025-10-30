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
#include "actor_login.h"
#include "account.h"

namespace ngl
{
	account::account() :
		m_max_accountid(1)
	{
	}

	void account::related_actorid()
	{
		set_actorid(nguid::make());
	}

	void account::initdata()
	{
		log_error()->print("actor_login###loaddb_finish {}", data());
		foreach([this](data_modified<pbdb::db_account>& dbaccount)
			{
				data_modified_return_get(lpdbaccount, dbaccount);
				i16_area larea = lpdbaccount->marea();
				const std::string& laccount = lpdbaccount->maccount();
				m_areaofaccount[larea].m_data[laccount] = &dbaccount;
				m_max_accountid = std::max(m_max_accountid, nguid::actordataid(lpdbaccount->mid()));
			});
	}

	int32_t& account::max_accountid()
	{
		return m_max_accountid;
	}

	std::map<std::string, data_modified<pbdb::db_account>*>& account::accountbyarea(int area)
	{
		return m_areaofaccount[area].m_data;
	}
}//namespace ngl