#pragma once

#include "gateway_info.h"
#include "actor_manage.h"
#include "actor_role.h"
#include "db_manage.h"
#include "ndbclient.h"
#include "db_data.h"
#include "db_pool.h"
#include "net.h"
#include "db.h"

namespace ngl
{
	class actor_gatewayg2c : public actor
	{
		gateway_info m_info;

		actor_gatewayg2c(const actor_gatewayg2c&) = delete;
		actor_gatewayg2c& operator=(const actor_gatewayg2c&) = delete;

		actor_gatewayg2c();
	public:
		friend class actor_instance<actor_gatewayg2c>;
		static actor_gatewayg2c& getInstance()
		{
			return actor_instance<actor_gatewayg2c>::instance();
		}

		virtual ~actor_gatewayg2c();

		static void nregister();

		static ENUM_ACTOR actor_type()
		{
			return ACTOR_GATEWAY_G2C;
		}

		static i64_actorid actorid(i32_actordataid aactordataid)
		{
			return nguid::make(ACTOR_GATEWAY_G2C, ttab_servers::tab()->m_area, aactordataid);
		}

		template <EPROTOCOL_TYPE TYPE, typename T>
		bool handle(message<np_actor_forward<T, TYPE, true, ngl::forward>>& adata)
		{
			auto lparm = adata.get_data();
			auto lpack = adata.m_pack;
			// Game->Gate  需要把这个消息传递给Client服务器
			gateway_socket* info = nullptr;
			std::map<i32_sessionid, i64_actorid> lmap;
			if (lparm->m_uid.empty())
				return true;
			if (lparm->m_area[0] == -1 && lparm->m_uid[0] == -1)
			{
				// 发给所有客户端
				for (const auto& itemfor1 : m_info.info())
				{
					for (const auto& itemfor2 : itemfor1.second)
					{
						i64_actorid lactorid = nguid::make(ACTOR_ROBOT, itemfor2.second.m_area, itemfor2.second.m_dataid);
						lmap.insert(std::make_pair(itemfor2.second.m_socket, lactorid));
					}
				}
			}
			else if (lparm->m_area[0] != -1 && lparm->m_uid[0] == -1)
			{
				// 发给指定区服上的所有客户端
				auto itemitor = m_info.info().find(lparm->m_area[0]);
				if (itemitor != m_info.info().end())
				{
					for (const auto& itemfor1 : itemitor->second)
					{
						i64_actorid lactorid = nguid::make(ACTOR_ROBOT, itemfor1.second.m_area, itemfor1.second.m_dataid);
						lmap.insert(std::make_pair(itemfor1.second.m_socket,lactorid));
					}
				}
			}
			else
			{
				for (int i = 0; i < lparm->m_uid.size() && i < lparm->m_area.size(); ++i)
				{
					info = m_info.get(lparm->m_area[i], lparm->m_uid[i]);
					if (info == nullptr)
						continue;
					i64_actorid lactorid = nguid::make(ACTOR_ROBOT, lparm->m_area[i], lparm->m_uid[i]);
					lmap.insert(std::make_pair(info->m_socket, lactorid));
				}
			}
			np_actor_forward<T, TYPE, false, ngl::forward> ltemp(*lparm);
			nets::sendmore(lmap, ltemp, lpack->m_head.get_request_actor());
			return true;
		}

		bool handle(message<np_actor_gatewayinfo_updata>& adata);
	};

}//namespace ngl

