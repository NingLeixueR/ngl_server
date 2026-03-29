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
#include "actor/actor_base/core/actor_manage.h"
#include "actor/actor_base/core/ndbclient.h"
#include "tools/db/sql/mysql/nmysql.h"
#include "tools/db/sql/db_data.h"
#include "net/nnet.h"
#include "net/udp/kcp/nkcp.h"

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

		bool timer_handle([[maybe_unused]] const message<np_timerparm>& adata);

		template <typename T>
		bool handle_socket(const np_actor_forward<T, forward_g2c<forward>>* aparm, const pack* apack)
		{
			const auto& ldata = aparm->m_data;
			const auto& luids = ldata.m_uid;
			const auto& lareas = ldata.m_area;
			const std::size_t luid_size = luids.size();
			if (luid_size != lareas.size())
			{
				log_error()->print("actor_gatewayg2c uidsize[{}]!=areasize[{}]", luid_size, lareas.size());
				return true;
			}
			std::map<i32_sessionid, i64_actorid> lmap;
			const i16_area larea0 = lareas[0];
			const i32_actordataid ldata0 = luids[0];
			if (ldata0 == nguid::none_actordataid())
			{
				if (larea0 == nguid::none_area())
				{
					get_allclient(lmap);
				}
				else
				{// if(larea0 != nguid::none_area())
					get_allclientbyarea(lmap, larea0);
				}
			}
			else
			{
				for (std::size_t lidx = 0; lidx < luid_size; ++lidx)
				{
					const i16_area larea = lareas[lidx];
					const i32_actordataid ldataid = luids[lidx];
					gateway_socket* linfo = m_info.get(larea, ldataid);
					if (linfo == nullptr || linfo->m_socket <= 0)
					{
						continue;
					}
					const i64_actorid lactorid = nguid::make(ACTOR_ROBOT, larea, ldataid);
					lmap.emplace(linfo->m_socket, lactorid);
				}
			}			
			return nnet::instance().send<forward, T>(lmap, ldata.m_data, apack->m_head->get_request_actor());
		}

		template <typename T>
		bool handle_kcp(const np_actor_forward<T, forward_g2c<forward>>* aparm, const pack* apack)
		{
			const auto& ldata = aparm->m_data;
			const auto& luids = ldata.m_uid;
			const auto& lareas = ldata.m_area;
			const std::size_t luid_size = luids.size();
			if (luids.empty() || lareas.empty() || luid_size != lareas.size())
			{
				log_error()->print("actor_gatewayg2c uidsize[{}]!=areasize[{}]", luid_size, lareas.size());
				return true;
			}
			auto lkcp = nkcp::instance().serkcp(pbnet::KCP_GATEWAY, nconfig.tcount());
			std::shared_ptr<pack> lsendpack = ngl::net_pack<T>::npack(&nnet::instance().pool(), ldata.m_data, apack->m_head->get_request_actor(), 0);
			if (lsendpack == nullptr)
			{
				return true;
			}
			const i16_area larea0 = lareas[0];
			const i32_actordataid ldata0 = luids[0];
			if (ldata0 == nguid::none_actordataid())
			{
				if (larea0 == nguid::none_area())
				{
					lkcp->send_server(lsendpack);
				}
				else
				{// if(larea0 != nguid::none_area())
					lkcp->sendpackbyarea(larea0, lsendpack);
				}
			}
			else if (luid_size == 1)
			{
				const i64_actorid lactorid = nguid::make(ACTOR_ROBOT, larea0, ldata0);
				lkcp->send_server(lactorid, lsendpack);
			}
			else
			{
				std::set<i64_actorid> lids;
				for (std::size_t lidx = 0; lidx < luid_size; ++lidx)
				{
					lids.emplace(nguid::make(ACTOR_ROBOT, lareas[lidx], luids[lidx]));
				}
				lkcp->send_server(lids, lsendpack);
			}
			return true;
		}

		template <typename T>
		bool handle([[maybe_unused]] const message<np_actor_forward<T, forward_g2c<forward>>>& adata)
		{
			const auto* lparm = adata.get_data();
			const pack* lpack = adata.get_pack();
			if (lparm->m_data.m_uid.empty())
			{
				return true;
			}
			// Game->Gate need tothis message toClientserver
			if (lparm->m_data.m_protocol == ENET_KCP)
			{
				return handle_kcp(lparm, lpack);
			}
			return handle_socket(lparm, lpack);
		}

		bool handle([[maybe_unused]] const message<np_actor_gatewayinfo_updata>& adata);
	};
}//namespace ngl
