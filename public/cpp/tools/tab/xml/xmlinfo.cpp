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
#include "nprotocol.h"
#include "xmlinfo.h"
#include "tools.h"
#include "nlog.h"
#include "nlog.h"

#include <string>

namespace ngl
{
	void xmlinfo::plog()
	{
		log_error()->print("xmlinfo:{}", m_data);
	}
	
	bool xmlinfo::find(const char* akey, bool& adata)
	{
		std::string* lp = tools::findmap<std::string, std::string>(m_data, akey);
		if (lp == nullptr)
		{
			return false;
		}
		adata = *lp == "true" || *lp == "TRUE";
		return true;
	}

	bool xmlinfo::find(const char* akey, std::string& adata)
	{
		std::string* lp = tools::findmap<std::string, std::string>(m_data, akey);
		if (lp == nullptr)
		{
			return false;
		}
		adata = *lp;
		return true;
	}

	void xmlinfo::foreach(const std::function<void(const std::pair<const std::string, std::string>&)>& afun)
	{
		for (const auto& item : m_data)
		{
			afun(item);
		}
	}
}// namespace ngl