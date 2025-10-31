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

#include "nactor_auto.h"
#include "manage_csv.h"
#include "localtime.h"
#include "actor.h"

#include <string>
#include <map>

namespace ngl
{
	class actor_brief;

	class briefdb : 
		public tdb_brief::db_modular
	{
		briefdb(const briefdb&) = delete;
		briefdb& operator=(const briefdb&) = delete;

	public:
		briefdb()
		{
		}

		void related_actorid() final
		{
			set_actorid(nguid::make());
		}

		// û�о����
		void update_brief(const pbdb::db_brief* adata)
		{
			*(data()[adata->mid()].get()) = *adata;
		}

		virtual void initdata()
		{
			log_error()->print("actor_brief###loaddb_finish\n {}", data());
		}

		void update(const std::vector<pbdb::db_brief>& m_vecinfo)
		{
			for (const pbdb::db_brief& item : m_vecinfo)
			{
				update_brief(&item);
			}
		}
	};
}// namespace ngl

mk_formatter(pbdb::db_brief)