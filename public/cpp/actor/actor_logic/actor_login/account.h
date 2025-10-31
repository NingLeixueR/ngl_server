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

#include "nactor_auto.h"

namespace ngl
{
	class account : 
		public tdb_account::db_modular
	{
		account(const account&) = delete;
		account& operator=(const account&) = delete;

		struct accountbyname
		{
			std::map<std::string, data_modified<pbdb::db_account>*> m_data;
		};
		std::map<i16_area, accountbyname> m_areaofaccount;
		
		int32_t m_max_accountid;
	public:
		account();

		void related_actorid() final;

		virtual void initdata();

		int32_t& max_accountid();

		std::map<std::string, data_modified<pbdb::db_account>*>& accountbyarea(int area);
	};
}//namespace ngl

mk_formatter(pbdb::db_account)