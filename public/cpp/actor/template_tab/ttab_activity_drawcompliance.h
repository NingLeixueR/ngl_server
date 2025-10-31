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
	class ttab_activity_drawcompliance :
		public manage_csv<tab_activity_drawcompliance>
	{
		ttab_activity_drawcompliance(const ttab_activity_drawcompliance&) = delete;
		ttab_activity_drawcompliance& operator=(const ttab_activity_drawcompliance&) = delete;

		ttab_activity_drawcompliance()
		{
			allcsv::loadcsv(this);
		}

		void reload()final
		{
			std::cout << "[ttab_activity_drawcompliance] reload" << std::endl;
		}

	public:
		using type_tab = tab_activity_drawcompliance;

		static ttab_activity_drawcompliance& instance()
		{
			static ttab_activity_drawcompliance ltemp;
			return ltemp;
		}

		const std::map<int, tab_activity_drawcompliance>& tablecsv()
		{
			ttab_activity_drawcompliance* ttab = allcsv::get<ttab_activity_drawcompliance>();
			if (ttab == nullptr)
			{
				tools::no_core_dump();
			}
			return ttab->m_tablecsv;
		}

		const tab_activity_drawcompliance* tab(int32_t aid)
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
