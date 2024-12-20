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
			return nguid::make(ACTOR_GATEWAY_G2C, tab_self_area, aactordataid);
		}

		void get_allclient(std::map<i32_sessionid, i64_actorid>& amap)
		{
			for (const auto& itemfor1 : m_info.info())
			{
				for (const auto& itemfor2 : itemfor1.second)
				{
					i64_actorid lactorid = nguid::make(ACTOR_ROBOT, itemfor2.second.m_area, itemfor2.second.m_dataid);
					amap.insert(std::make_pair(itemfor2.second.m_socket, lactorid));
				}
			}
		}

		void get_allclientbyarea(std::map<i32_sessionid, i64_actorid>& amap, i16_area aarea)
		{
			auto itemitor = m_info.info().find(aarea);
			if (itemitor != m_info.info().end())
			{
				for (const auto& itemfor1 : itemitor->second)
				{
					i64_actorid lactorid = nguid::make(ACTOR_ROBOT, itemfor1.second.m_area, itemfor1.second.m_dataid);
					amap.insert(std::make_pair(itemfor1.second.m_socket, lactorid));
				}
			}
		}

		template <EPROTOCOL_TYPE TYPE, typename T>
		bool handle(const message<np_actor_forward<T, TYPE, true, ngl::forward>>& adata)
		{
			auto lparm = adata.get_data();
			auto lpack = adata.m_pack;
			// Game->Gate  需要把这个消息传递给Client服务器
			gateway_socket* info = nullptr;
			std::map<i32_sessionid, i64_actorid> lmap;
			if (lparm->m_uid.empty())
			{
				return true;
			}
			if (lparm->m_area[0] == -1 && lparm->m_uid[0] == -1)
			{
				// 获取所有客户端
				get_allclient(lmap);
			}
			else if (lparm->m_area[0] != -1 && lparm->m_uid[0] == -1)
			{
				// 获取指定区服上的所有客户端
				get_allclientbyarea(lmap, lparm->m_area[0]);
			}
			else
			{
				int32_t luidsize = lparm->m_uid.size();
				int32_t lareasize = lparm->m_area.size();
				if (luidsize != lareasize)
				{
					log_error()->print("actor_gatewayg2c uidsize[{}]!=areasize[{}]", luidsize, lareasize);
					return true;
				}
				for (int i = 0; i < luidsize; ++i)
				{
					i16_area larea = lparm->m_area[i];
					i32_actordataid ldataid = lparm->m_uid[i];
					info = m_info.get(larea, ldataid);
					if (info == nullptr)
					{
						continue;
					}
					i64_actorid lactorid = nguid::make(ACTOR_ROBOT, larea, ldataid);
					lmap.insert(std::make_pair(info->m_socket, lactorid));
				}
			}
			np_actor_forward<T, TYPE, false, ngl::forward> ltemp(*lparm);
			nets::sendmore(lmap, ltemp, lpack->m_head.get_request_actor());
			return true;
		}

		bool handle(const message<np_actor_gatewayinfo_updata>& adata);
	};

}//namespace ngl

