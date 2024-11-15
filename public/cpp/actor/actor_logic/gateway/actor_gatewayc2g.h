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
	class actor_gatewayc2g : public actor
	{
		actor_gatewayc2g(const actor_gatewayc2g&) = delete;
		actor_gatewayc2g& operator=(const actor_gatewayc2g&) = delete;

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

		static ENUM_ACTOR actor_type()
		{
			return ACTOR_GATEWAY_C2G;
		}

		static i64_actorid actorid(i32_actordataid aactordataid)
		{
			return nguid::make(ACTOR_GATEWAY_C2G, tab_self_area, aactordataid);
		}

		template <EPROTOCOL_TYPE TYPE, typename T>
		bool handle(const message<np_actor_forward<T, TYPE, false, ngl::forward>>& adata)
		{
			//Client->Gate  需要把这个消息传递给Game服务器
			//adata.m_uid == socket id
			auto lpram = adata.get_data();
			auto lpack = adata.m_pack;
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
				return false;
			np_actor_forward<T, TYPE, true, ngl::forward> ltemp(*lpram);
			ltemp.m_uid.push_back(info->m_dataid);
			ltemp.m_area.push_back(info->m_area);
			i64_actorid lactorid = nguid::make(ACTOR_ROLE, info->m_area, info->m_dataid);
			nets::sendbyserver(info->m_gameid, ltemp, lactorid, lpack->m_head.get_request_actor());
			return true;
		}

		bool handle(const message<np_actor_gatewayinfo_updata>& adata);
	};
}//namespace ngl

