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

#include "gateway_info.h"
#include "actor_manage.h"
#include "actor_role.h"
#include "db_manage.h"
#include "ndbclient.h"
#include "db_data.h"
#include "db_pool.h"
#include "ntcp.h"
#include "db.h"

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

		// # 获取所有客户端
		void get_allclient(std::map<i32_sessionid, i64_actorid>& amap);

		// # 获取指定区服的所有客户端
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
				///nets::serkcp(pbnet::KCP_GATEWAY, nconfig.tcount())->send_server(lsendpack);
			}
			else if (aparm->m_data.m_area[0] != nguid::none_area() && aparm->m_data.m_uid[0] == nguid::none_actordataid())
			{
				///nets::serkcp(pbnet::KCP_GATEWAY, nconfig.tcount())->sendpackbyarea(aparm->m_data.m_area[0], lsendpack);
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
				//nets::serkcp(pbnet::KCP_GATEWAY, nconfig.tcount())->send_server(lids, lsendpack);
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
			// Game->Gate  需要把这个消息传递给Client服务器
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

