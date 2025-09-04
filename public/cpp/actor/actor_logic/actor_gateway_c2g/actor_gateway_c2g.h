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

		virtual void loaddb_finish(bool adbishave);

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
				return false;
			}

			i64_actorid lactorid = nguid::make(ACTOR_ROLE, info->m_area, info->m_dataid);
			nets::sendbyserver(info->m_gameid, lpram->m_data.m_data, lactorid, lpack->m_head.get_request_actor());
			return true;
		}

		bool handle(const message<np_actor_gatewayinfo_updata>& adata);
	};
}//namespace ngl