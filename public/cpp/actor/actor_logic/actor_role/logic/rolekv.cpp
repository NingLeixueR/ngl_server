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
#include "actor_role.h"
#include "rolekv.h"

namespace ngl
{
	bool rolekv::value(const char* akey, int8_t& adata)
	{
		return number_value(akey, adata);
	}

	bool rolekv::value(const char* akey, uint8_t& adata)
	{
		return number_value(akey, adata);
	}

	bool rolekv::value(const char* akey, int32_t& adata)
	{
		return number_value(akey, adata);
	}

	bool rolekv::value(const char* akey, uint32_t& adata)
	{
		return number_value(akey, adata);
	}

	bool rolekv::value(const char* akey, int64_t& adata)
	{
		return number_value(akey, adata);
	}

	bool rolekv::value(const char* akey, uint64_t& adata)
	{
		return number_value(akey, adata);
	}

	bool rolekv::value(const char* akey, float& adata)
	{
		return number_value(akey, adata);
	}

	bool rolekv::value(const char* akey, double& adata)
	{
		return number_value(akey, adata);
	}
}//namespace ngl