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
	class ttab_activityopenserver :
		public manage_csv<tab_activityopenserver>
	{
		ttab_activityopenserver(const ttab_activityopenserver&) = delete;
		ttab_activityopenserver& operator=(const ttab_activityopenserver&) = delete;

		ttab_activityopenserver()
		{
			allcsv::loadcsv(this);
		}

		void reload()final
		{
			std::cout << "[ttab_activityopenserver] reload" << std::endl;
		}

	public:
		using type_tab = tab_activityopenserver;

		static ttab_activityopenserver& instance()
		{
			static ttab_activityopenserver ltemp;
			return ltemp;
		}

		const std::map<int, tab_activityopenserver>& tablecsv()
		{
			ttab_activityopenserver* ttab = allcsv::get<ttab_activityopenserver>();
			if (ttab == nullptr)
			{
				tools::no_core_dump();
			}
			return ttab->m_tablecsv;
		}

		const tab_activityopenserver* tab(int32_t aid)
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
