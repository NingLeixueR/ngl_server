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

#include "xmlinfo.h"
#include "type.h"
#include "nlog.h"

#include <functional>
#include <string>
#include <map>

namespace ngl
{
	class xmlprotocol
	{
		static std::map<std::string, int32_t> m_protocol;

		static void read(const std::string& axmlname);
	public:
		// # ��xml�м�������
		static void load();

		// # ��ȡЭ���
		static int32_t protocol(const std::string& aname);

		// # makeproto���ߵ��� ���Բ���Ҫ����
		static int32_t free_protocol();
	};
}// namespace ngl