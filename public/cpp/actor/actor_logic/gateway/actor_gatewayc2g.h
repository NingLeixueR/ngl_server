#pragma once

#include "actor_manage.h"
#include "gateway_info.h"
#include "ndbclient.h"
#include "db_manage.h"
#include "db_pool.h"
#include "db_data.h"
#include "db.h"
#include "net.h"

namespace ngl
{

	class actor_gatewayc2g : public actor
	{
		gateway_info m_info;
		actor_gatewayc2g();
	public:
		friend class actor_instance<actor_gatewayc2g>;
		static actor_gatewayc2g& getInstance()
		{
			return actor_instance<actor_gatewayc2g>::instance();
		}

		virtual ~actor_gatewayc2g();

		static void nregister();

		template <EPROTOCOL_TYPE TYPE, typename T>
		bool handle(message<actor_forward<T, TYPE, false, ngl::forward>>& adata)
		{
			//Client->Gate  需要把这个消息传递给Game服务器
			//adata.m_uid == socket id
			auto lpram = adata.m_data;
			auto lpack = adata.m_pack;
			gateway_socket* info = nullptr;
			if (rebot_test::is_test)
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
				return false;
			actor_forward<T, TYPE, true, ngl::forward> ltemp(*lpram);
			ltemp.m_uid.push_back(info->m_dataid);
			ltemp.m_area.push_back(info->m_area);
			nserver->send_server(info->m_gameid, ltemp, nguid::make(ACTOR_ROLE, info->m_area, info->m_dataid), lpack->m_head.get_request_actor());
			return true;
		}

		bool handle(message<actor_gateway_info_updata>& adata);
	};
}//namespace ngl

