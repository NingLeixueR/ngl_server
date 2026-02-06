/*
* Copyright (c) [2020-2025] NingLeixueR
* 
* 项目名称：ngl_server
* 项目地址：https://github.com/NingLeixueR/ngl_server
* 
* 本文件是 ngl_server 项目的一部分，遵循 MIT 开源协议发布。
* 您可以按照协议规定自由使用、修改和分发本项目，包括商业用途，
* 但需保留原始版权和许可声明。
* 
* 许可详情参见项目根目录下的 LICENSE 文件：
* https://github.com/NingLeixueR/ngl_server/blob/main/LICENSE
*/
#pragma once

#include "nmysql_manage.h"
#include "actor_manage.h"
#include "actor_create.h"
#include "nmysql_pool.h"
#include "nprotocol.h"
#include "ndbclient.h"
#include "logprintf.h"
#include "db_data.h"
#include "ntimer.h"
#include "nmysql.h"
#include "ntcp.h"

#include <fstream>
#include <memory>

namespace ngl
{
	struct log_timerparm
	{
		enum etype
		{
			e_logflush,
			e_create,
		};
		etype m_type;

		explicit log_timerparm(etype atype) :
			m_type(atype)
		{}
	};

	class actor_log : 
		public actor
	{
		actor_log() = delete;
		actor_log(const actor_log&) = delete;
		actor_log& operator=(const actor_log&) = delete;

		std::map<ELOG_TYPE, std::shared_ptr<logfile>> m_logarr;
	public:
		explicit actor_log(int32_t atype);

		virtual ~actor_log() = default;

		static ENUM_ACTOR actor_type();

		static i64_actorid actorid(i32_actordataid aid);

		virtual void init();

		virtual void loaddb_finish(pbdb::ENUM_DB atype, enum_dbstat astat);

		static void nregister();

		bool timer_handle(const message<np_timerparm>& adata);

		bool handle(const message<np_logitem>& adata);
	};
}//namespace ngl
