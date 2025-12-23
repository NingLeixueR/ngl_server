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

#include "actor_manage.h"
#include "gateway_info.h"
#include "ndbclient.h"
#include "db_manage.h"
#include "db_pool.h"
#include "db_data.h"
#include "net.h"
#include "db.h"

namespace ngl
{
	class actor_gateway_c2g 
		: public actor
	{
		actor_gateway_c2g(const actor_gateway_c2g&) = delete;
		actor_gateway_c2g& operator=(const actor_gateway_c2g&) = delete;

		gateway_info m_info;
		actor_gateway_c2g();
	public:
		friend class actor_instance<actor_gateway_c2g>;
		static actor_gateway_c2g& instance()
		{
			return actor_instance<actor_gateway_c2g>::instance();
		}

		virtual ~actor_gateway_c2g() = default;

		static ENUM_ACTOR actor_type();


		static i64_actorid actorid(i32_actordataid aactordataid);

		virtual void init();

		virtual void loaddb_finish(pbdb::ENUM_DB atype, enum_dbstat astat);

		static void nregister();

		bool timer_handle(const message<np_timerparm>& adata);

		bool handle(const message<np_arg_null>&);

		template <typename T>
		bool handle(const message<np_actor_forward<T, forward_c2g<forward>>>& adata)
		{
			//Client->Gate  需要把这个消息传递给Game服务器
			//adata.m_uid == socket id
			auto lpram = adata.get_data();
			auto lpack = adata.get_pack();
			gateway_socket* info = nullptr;
			if (sysconfig::robot_test())
			{
				i64_actorid lactorid = lpack->m_head.get_request_actor();
				nguid lguid(lactorid);
				info = m_info.get(lguid.area(), lguid.actordataid());
			}
			else
			{
				info = m_info.get(lpack->m_id);//m_info.get(adata.m_uid[0]);
			}
			if (info == nullptr)
			{
				log_error()->print("actor_gateway_c2g fail info == nullptr");
				return false;
			}

			nets::send_server<forward, T>(info->m_gameid, lpram->m_data.m_data, nguid::make(ACTOR_ROLE, info->m_area, info->m_dataid), lpack->m_head.get_request_actor());
			return true;
		}

		bool handle(const message<np_actor_gatewayinfo_updata>& adata);
	};
}//namespace ngl