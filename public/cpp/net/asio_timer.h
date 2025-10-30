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

namespace ngl
{
	// # ���������粻�����¼�  �뼶��ʱ��
	class asio_timer
	{
		asio_timer(const asio_timer&) = delete;
		asio_timer& operator=(const asio_timer&) = delete;

		asio_timer() {}

		static time_wheel m_wheel;
	public:
		static time_wheel& wheel()
		{
			return m_wheel;
		}
	};
}// namespace ngl