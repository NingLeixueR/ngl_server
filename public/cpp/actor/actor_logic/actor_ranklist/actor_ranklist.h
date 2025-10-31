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
#include "db_manage.h"
#include "ndbclient.h"
#include "nprotocol.h"
#include "ranklist.h"
#include "db_data.h"
#include "db_pool.h"
#include "ntimer.h"
#include "notice.h"
#include "net.pb.h"
#include "net.h"
#include "db.h"

namespace ngl
{
	class actor_ranklist : 
		public actor
	{
		actor_ranklist(const actor_ranklist&) = delete;
		actor_ranklist& operator=(const actor_ranklist&) = delete;

		ranklist m_ranklist;

		actor_ranklist();
	public:
		friend class actor_instance<actor_ranklist>;
		static actor_ranklist& instance()
		{
			return actor_instance<actor_ranklist>::instance();
		}

		virtual ~actor_ranklist() = default;

		static ENUM_ACTOR actor_type();

		static i64_actorid actorid();

		virtual void init();

		virtual void loaddb_finish(bool adbishave);

		virtual void erase_actor_before();

		static void nregister();

		using handle_cmd = cmd<actor_ranklist, std::string, int, ngl::njson_read&>;

		bool timer_handle(const message<np_timerparm>& adata);

		bool handle(const message<np_arg_null>&);
		
		bool handle(const message<mforward<np_gm>>& adata);
		// [����]��ȡ���а�
		bool handle(const message<mforward<pbnet::PROBUFF_NET_RANKLIST>>& adata);
		bool handle(const message<np_activityrank_operator>& adata);
		bool handle(const message<np_get_rank>& adata);
	};
}// namespace ngl
