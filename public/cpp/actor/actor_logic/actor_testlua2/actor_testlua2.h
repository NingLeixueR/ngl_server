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

#include "actor_manage.h"
#include "actor_create.h"
#include "ndb_modular.h"
#include "nsp_server.h"
#include "nsp_write.h"
#include "nsp_read.h"
#include "ndbclient.h"
#include "nprotocol.h"
#include "db_manage.h"
#include "db_pool.h"
#include "db_data.h"
#include "ntimer.h"
#include "drop.h"
#include "nlog.h"
#include "net.h"
#include "db.h"

namespace ngl
{
	class actor_testlua2 :
		public actor
	{
		actor_testlua2(const actor_testlua2&) = delete;
		actor_testlua2& operator=(const actor_testlua2&) = delete;

		actor_testlua2();
	public:
		friend class actor_instance<actor_testlua2>;
		static actor_testlua2& instance()
		{
			return actor_instance<actor_testlua2>::instance();
		}
		virtual ~actor_testlua2() = default;

		static ENUM_ACTOR actor_type();

		static i64_actorid actorid();

		virtual void init();

		virtual void erase_actor_before();

		virtual void loaddb_finish(bool adbishave);

		static void nregister();

		bool timer_handle(const message<np_timerparm>& adata);

		bool handle(const message<np_arg_null>&);
	};
}// namespace ngl
