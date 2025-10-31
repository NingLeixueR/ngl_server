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
#include "time_consuming.h"
#include "nprotocol.h"

namespace ngl
{
	time_consuming::time_consuming(const std::string& aname):
		m_name(aname),
		m_beg(0)
	{
	}

	void time_consuming::consuming_start()
	{
		m_beg = time_wheel::getms();
	}

	void time_consuming::consuming_finish()
	{
		int64_t lconsuming = time_wheel::getms() - m_beg;
		bool lerror = lconsuming > sysconfig::consumings();
		(lerror ? log_error() : log_info())->print("time consuming [{}:{}]", m_name, lconsuming);
	}
}//namespace ngl