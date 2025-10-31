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

#include "manage_csv.h"
#include "type.h"
#include "xml.h"

namespace ngl
{
	class ttab_synthesis :
		public manage_csv<tab_synthesis>
	{
		ttab_synthesis(const ttab_synthesis&) = delete;
		ttab_synthesis& operator=(const ttab_synthesis&) = delete;

		ttab_synthesis()
		{
			allcsv::loadcsv(this);
		}

		void reload()final
		{
			std::cout << "[ttab_synthesis] reload" << std::endl;
		}

	public:
		using type_tab = tab_synthesis;

		static ttab_synthesis& instance()
		{
			static ttab_synthesis ltemp;
			return ltemp;
		}

		const std::map<int, tab_synthesis>& tablecsv()
		{
			ttab_synthesis* ttab = allcsv::get<ttab_synthesis>();
			if (ttab == nullptr)
			{
				tools::no_core_dump();
			}
			return ttab->m_tablecsv;
		}

		const tab_synthesis* tab(int32_t aid)
		{
			auto itor = tablecsv().find(aid);
			if (itor == tablecsv().end())
			{
				return nullptr;
			}
			return &itor->second;
		}
	};
}//namespace ngl
