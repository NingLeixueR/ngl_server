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
	class ttab_activityalways :
		public manage_csv<tab_activityalways>
	{
		ttab_activityalways(const ttab_activityalways&) = delete;
		ttab_activityalways& operator=(const ttab_activityalways&) = delete;

		ttab_activityalways()
		{
			allcsv::loadcsv(this);
		}

		void reload()final
		{
			std::cout << "[ttab_activityalways] reload" << std::endl;
		}

	public:
		using type_tab = tab_activityalways;

		static ttab_activityalways& instance()
		{
			static ttab_activityalways ltemp;
			return ltemp;
		}

		const std::map<int, tab_activityalways>& tablecsv()
		{
			ttab_activityalways* ttab = allcsv::get<ttab_activityalways>();
			if (ttab == nullptr)
			{
				tools::no_core_dump();
			}
			return ttab->m_tablecsv;
		}

		const tab_activityalways* tab(int32_t aid)
		{
			auto& lmap = tablecsv();
			auto itor = lmap.find(aid);
			if (itor == lmap.end())
			{
				return nullptr;
			}
			return &itor->second;
		}
	};
}//namespace ngl
