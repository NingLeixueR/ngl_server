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
#include "operator_file.h"
#include "nprotocol.h"
#include "tools.h"
#include "nlog.h"
#include "csv.h"

namespace ngl
{
	bool rcsv::read(const std::string& aname, std::string& averify)
	{
		//# ��ȡ�ļ�
		readfile lrf(aname);
		//# ����ǰ3�б�ͷ
		lrf.jumpbegin(3, true);

		if (lrf.readcurrent(m_data) == false)
		{
			std::cout << std::format("loadcsv fail #{}", aname) << std::endl;
			return false;
		}
		averify = tools::md5(m_data);
		std::cout << std::format("loadcsv#{}#{}", aname, averify) << std::endl;
		return true;
	}
}// namespace ngl