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

#include "actor.h"

namespace ngl
{
	class actor_csvclient 
		: public actor
	{
		actor_csvclient(const actor_csvclient&) = delete;
		actor_csvclient& operator=(const actor_csvclient&) = delete;

		actor_csvclient();
	public:
		friend class actor_instance<actor_csvclient>;
		static actor_csvclient& instance()
		{
			return actor_instance<actor_csvclient>::instance();
		}

		virtual ~actor_csvclient() = default;

		static ENUM_ACTOR actor_type();

		static i64_actorid actorid();

		virtual void init();

		virtual void loaddb_finish(bool adbishave);

		static void nregister();

		bool timer_handle(const message<np_timerparm>& adata);

		bool handle(const message<np_arg_null>&);
		bool handle(const message<np_actor_reloadcsv>& adata);
	};
}//namespace ngl
