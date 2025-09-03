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
	class actor_gateway_g2c : public actor
	{
		gateway_info m_info;

		actor_gateway_g2c(const actor_gateway_g2c&) = delete;
		actor_gateway_g2c& operator=(const actor_gateway_g2c&) = delete;

		actor_gateway_g2c();
	public:
		friend class actor_instance<actor_gateway_g2c>;
		static actor_gateway_g2c& instance()
		{
			return actor_instance<actor_gateway_g2c>::instance();
		}

		virtual ~actor_gateway_g2c() = default;

		static ENUM_ACTOR actor_type();

		static i64_actorid actorid(i32_actordataid aactordataid);

		virtual void init();

		virtual void loaddb_finish(bool adbishave);

		static void nregister();

		void get_allclient(std::map<i32_sessionid, i64_actorid>& amap);

		void get_allclientbyarea(std::map<i32_sessionid, i64_actorid>& amap, i16_area aarea);

		bool timer_handle(const message<np_timerparm>& adata);

		bool handle(const message<np_arg_null>&);

		template <typename T>
		bool handle(const message<np_actor_forward<T, forward_g2c<forward>>>& adata)
		{
			auto lparm = adata.get_data();
			auto lpack = adata.get_pack();
			// Game->Gate  需要把这个消息传递给Client服务器
			gateway_socket* info = nullptr;
			std::map<i32_sessionid, i64_actorid> lmap;
			if (lparm->m_data.m_uid.empty())
			{
				return true;
			}
			if (lparm->m_data.m_area[0] == -1 && lparm->m_data.m_uid[0] == -1)
			{
				// 获取所有客户端
				get_allclient(lmap);
			}
			else if (lparm->m_data.m_area[0] != -1 && lparm->m_data.m_uid[0] == -1)
			{
				// 获取指定区服上的所有客户端
				get_allclientbyarea(lmap, lparm->m_data.m_area[0]);
			}
			else
			{
				int32_t luidsize = (int32_t)lparm->m_data.m_uid.size();
				int32_t lareasize = (int32_t)lparm->m_data.m_area.size();
				if (luidsize != lareasize)
				{
					log_error()->print("actor_gatewayg2c uidsize[{}]!=areasize[{}]", luidsize, lareasize);
					return true;
				}
				for (int i = 0; i < luidsize; ++i)
				{
					i16_area larea = lparm->m_data.m_area[i];
					i32_actordataid ldataid = lparm->m_data.m_uid[i];
					info = m_info.get(larea, ldataid);
					if (info == nullptr)
					{
						continue;
					}
					i64_actorid lactorid = nguid::make(ACTOR_ROBOT, larea, ldataid);
					lmap.insert(std::make_pair(info->m_socket, lactorid));
				}
			}
			nets::sendmore(lmap, lparm->m_data.m_data, lpack->m_head.get_request_actor());
			return true;
		}

		bool handle(const message<np_actor_gatewayinfo_updata>& adata);
	};

}//namespace ngl

