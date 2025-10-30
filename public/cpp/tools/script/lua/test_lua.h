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
#include "serialize_lua.h"
#include "ndefine.h"

namespace ngl
{
	// # ����

	struct k0
	{
		int m_v1;
		double m_v2;
		std::string m_v3;

		dprotocol(k0, m_v1, m_v2, m_v3)
	};

	struct k1
	{
		int m_v1;
		double m_v2;
		std::string m_v3;
		std::vector<int32_t> m_v4;
		std::map<int32_t, std::string> m_v5;
		std::map<std::string, std::string> m_v6;
		std::map<std::string, k0> m_v7;

		dprotocol(k1, m_v1, m_v2, m_v3, m_v4, m_v5, m_v6, m_v7)
	};

	struct k2
	{
		int m_v1;
		k1 m_v2;

		dprotocol(k2, m_v1, m_v2)
	};
}//namespace ngl