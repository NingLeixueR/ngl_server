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

#include <vector>
#include <string>
#include <map>

#include "actor_role.h"
#include "tools.h"
#include "nlog.h"

namespace ngl
{
	class consume
	{
	public:
		static bool get(int atid, int acount, std::map<int32_t, int32_t>& amap);
		static bool check(actor_role* arole, std::map<int32_t, int32_t>& amap);
		static bool check(actor_role* arole, int atid, int acount);
		static bool use(actor_role* arole, int atid, int acount);
	};
}// namespace ngl
