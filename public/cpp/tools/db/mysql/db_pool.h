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

#include "db.h"

namespace ngl
{
	class db_pool
	{
		db_pool(const db_pool&) = delete;
		db_pool& operator=(const db_pool&) = delete;

		std::vector<db*> m_vec;

		db_pool(){}
	public:
		static db_pool& instance()
		{
			static db_pool ltemp;
			return ltemp;
		}

		// # ��ʼ��db���ӳ�:Ŀǰ���ӳ� ֻ֧�ֵ����ã�
		// # ����ζ�ŵ�������ֻ����һ�����ݿ���������
		void init(const dbarg& adbarg);

		// # �����߳�id��ȡ���ж�Ӧ������
		db* get(int32_t aindex);
	};
}// namespace ngl
