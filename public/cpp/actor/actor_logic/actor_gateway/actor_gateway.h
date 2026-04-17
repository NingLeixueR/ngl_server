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
// File overview: Gateway actor that bridges external client connections to internal actor routing.

#pragma once

#include "actor/actor_logic/actor_gateway/gateway_info.h"
#include "actor/actor_base/core/actor_manage.h"

namespace ngl
{
	class actor_gateway : 
		public actor
	{
		actor_gateway(const actor_gateway&) = delete;
		actor_gateway& operator=(const actor_gateway&) = delete;

		gateway_info m_info;

		actor_gateway();
	public:
		friend class actor_instance<actor_gateway>;
		static actor_gateway& instance() 
		{ 
			return actor_instance<actor_gateway>::instance(); 
		}

		virtual ~actor_gateway() = default;

		static ENUM_ACTOR actor_type();

		static i64_actorid actorid(i32_actordataid aactordataid);

		virtual void init();

		virtual void loaddb_finish(pbdb::ENUM_DB atype, enum_dbstat astat);

		static void nregister();

		// Notify actor_server of the mapping from actorid to gateway server id
		void sync_actorserver_gatewayid(const nguid& aguid, bool aisremove);

		void update_gateway_info(const std::shared_ptr<np_actor_gatewayinfo_updata>& ap);

		void session_close(gateway_socket* ainfo);

		bool timer_handle([[maybe_unused]] const message<np_timerparm>& adata);

		bool handle([[maybe_unused]] const message<np_gateway_close_session>& adata);

		// Actor_login notifies gateway of player login and the associated game server session
		bool handle([[maybe_unused]] const message<np_actorrole_login>& adata);

		// Get KCP session
		bool handle([[maybe_unused]] const message<np_actor_kcp>& adata);

		// Player switches game server process
		bool handle([[maybe_unused]] const message<np_actorswitch_process<np_actorswitch_process_role>>& adata);

		// Connection closed
		bool handle([[maybe_unused]] const message<np_actor_session_close>& adata);
		
		// Player login request
		bool handle([[maybe_unused]] const message<pbnet::PROBUFF_NET_ROLE_LOGIN>& adata);

		// Get KCP session
		bool handle([[maybe_unused]] const message<pbnet::PROBUFF_NET_KCPSESSION>& adata);
	};
}//namespace ngl