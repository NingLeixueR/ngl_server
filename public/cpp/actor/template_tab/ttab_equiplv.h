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
	class ttab_equiplv :
		public manage_csv<tab_equiplv>
	{
		ttab_equiplv(const ttab_equiplv&) = delete;
		ttab_equiplv& operator=(const ttab_equiplv&) = delete;

		ttab_equiplv()
		{
			allcsv::loadcsv(this);
		}

		void reload()final
		{
			std::cout << "[ttab_equiplv] reload" << std::endl;
		}

	public:
		using type_tab = tab_equiplv;

		static ttab_equiplv& instance()
		{
			static ttab_equiplv ltemp;
			return ltemp;
		}

		std::map<int, tab_equiplv>& tablecsv()
		{
			ttab_equiplv* ttab = allcsv::get<ttab_equiplv>();
			if (ttab == nullptr)
			{
				tools::no_core_dump();
			}
			return ttab->m_tablecsv;
		}

		const tab_equiplv* tab(int32_t aid)
		{
			auto lmap = tablecsv();
			auto itor = lmap.find(aid);
			if (itor == lmap.end())
			{
				return nullptr;
			}
			return &itor->second;
		}
	};
}//namespace ngl
