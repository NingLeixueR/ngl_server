/*
* Copyright (c) [2020-2025] NingLeixueR
* 
* Project name: ngl_server
* Project URL: https://github.com/NingLeixueR/ngl_server
* 
* This file is part of the ngl_server project and is distributed under the MIT License.
* You may use, modify, and distribute this project under the license, including commercial use,
* but you must retain the original copyright and license notice.
* 
* For license details, see the LICENSE file in the project root:
* https://github.com/NingLeixueR/ngl_server/blob/main/LICENSE
*/
// File overview: Declares interfaces for actor gateway g2c.

#pragma once

#include "actor/actor_logic/actor_gateway/gateway_info.h"
#include "actor/actor_logic/actor_role/actor_role.h"
#include "tools/db/sql/mysql/nmysql_manage.h"
#include "tools/db/sql/mysql/nmysql_pool.h"
#include "actor/actor_base/actor_manage.h"
#include "actor/actor_base/ndbclient.h"
#include "tools/db/sql/mysql/nmysql.h"
#include "tools/db/sql/db_data.h"
#include "net/udp/kcp/nkcp.h"
#include "net/tcp/ntcp.h"

namespace ngl
{
	class actor_gateway_g2c :
		public actor
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

		virtual void loaddb_finish(pbdb::ENUM_DB atype, enum_dbstat astat);

		static void nregister();

		// # Getallclient
		void get_allclient(std::map<i32_sessionid, i64_actorid>& amap);

		// # Getspecifiedarea allclient
		void get_allclientbyarea(std::map<i32_sessionid, i64_actorid>& amap, i16_area aarea);

		bool timer_handle(const message<np_timerparm>& adata);

		template <typename T>
		bool handle_tcp(const np_actor_forward<T, forward_g2c<forward>>* aparm, const pack* apack)
		{
			gateway_socket* info = nullptr;
			std::map<i32_sessionid, i64_actorid> lmap;
			if (aparm->m_data.m_area[0] == nguid::none_area() && aparm->m_data.m_uid[0] == nguid::none_actordataid())
			{
				get_allclient(lmap);
			}
			else if (aparm->m_data.m_area[0] != nguid::none_area() && aparm->m_data.m_uid[0] == nguid::none_actordataid())
			{
				get_allclientbyarea(lmap, aparm->m_data.m_area[0]);
			}
			else
			{
				auto luidsize = (int32_t)aparm->m_data.m_uid.size();
				auto lareasize = (int32_t)aparm->m_data.m_area.size();
				if (luidsize != lareasize)
				{
					log_error()->print("actor_gatewayg2c uidsize[{}]!=areasize[{}]", luidsize, lareasize);
					return true;
				}
				for (int i = 0; i < luidsize; ++i)
				{
					i16_area larea = aparm->m_data.m_area[i];
					i32_actordataid ldataid = aparm->m_data.m_uid[i];
					info = m_info.get(larea, ldataid);
					if (info == nullptr)
					{
						continue;
					}
					i64_actorid lactorid = nguid::make(ACTOR_ROBOT, larea, ldataid);
					lmap.insert(std::make_pair(info->m_socket, lactorid));
				}
			}
			ntcp::instance().send<forward, T>(lmap, aparm->m_data.m_data, apack->m_head.get_request_actor());
			return true;
		}

		template <typename T>
		bool handle_kcp(const np_actor_forward<T, forward_g2c<forward>>* aparm, const pack* apack)
		{
			std::shared_ptr<pack> lsendpack = ngl::net_pack<T>::npack(&ntcp::instance().pool(), aparm->m_data.m_data, apack->m_head.get_request_actor(), 0);
			if (lsendpack == nullptr)
			{
				return true;
			}
			if (aparm->m_data.m_area[0] == nguid::none_area() && aparm->m_data.m_uid[0] == nguid::none_actordataid())
			{
				nkcp::instance().serkcp(pbnet::KCP_GATEWAY, nconfig.tcount())->send_server(lsendpack);
			}
			else if (aparm->m_data.m_area[0] != nguid::none_area() && aparm->m_data.m_uid[0] == nguid::none_actordataid())
			{
				nkcp::instance().serkcp(pbnet::KCP_GATEWAY, nconfig.tcount())->sendpackbyarea(aparm->m_data.m_area[0], lsendpack);
			}
			else
			{
				auto luidsize = (int32_t)aparm->m_data.m_uid.size();
				auto lareasize = (int32_t)aparm->m_data.m_area.size();
				if (luidsize != lareasize)
				{
					log_error()->print("actor_gatewayg2c uidsize[{}]!=areasize[{}]", luidsize, lareasize);
					return true;
				}
				std::set<i64_actorid> lids;
				for (int i = 0; i < luidsize; ++i)
				{
					i16_area larea = aparm->m_data.m_area[i];
					i32_actordataid ldataid = aparm->m_data.m_uid[i];
					i64_actorid lactorid = nguid::make(ACTOR_ROBOT, larea, ldataid);
					lids.insert(lactorid);
				}
				nkcp::instance().serkcp(pbnet::KCP_GATEWAY, nconfig.tcount())->send_server(lids, lsendpack);
			}
			return true;
		}

		template <typename T>
		bool handle(const message<np_actor_forward<T, forward_g2c<forward>>>& adata)
		{
			auto lparm = adata.get_data();
			auto lpack = adata.get_pack();
			if (lparm->m_data.m_uid.empty())
			{
				return true;
			}
			// Game->Gate need tothis message toClientserver
			if (lparm->m_data.m_protocol == ENET_TCP)
			{
				return handle_tcp(lparm, lpack);
			}
			else if (lparm->m_data.m_protocol == ENET_KCP)
			{
				return handle_kcp(lparm, lpack);				
			}
			return true;
		}

		bool handle(const message<np_actor_gatewayinfo_updata>& adata);
	};
}//namespace ngl