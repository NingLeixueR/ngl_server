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

#include "time_wheel.h"
#include "nlog.h"

namespace ngl
{
	class time_consuming
	{
		time_consuming() = delete;
		time_consuming(const time_consuming&) = delete;
		time_consuming& operator=(const time_consuming&) = delete;

		std::string		m_name;
		int64_t			m_beg;
	public:
		time_consuming(const std::string& aname);
		void consuming_start();
		void consuming_finish();
	};
}//namespace ngl