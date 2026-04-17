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
// File overview: Client-to-gateway protocol actor that deserializes inbound client messages.

#pragma once

#include "actor/actor_logic/actor_gateway/gateway_info.h"
#include "actor/actor_base/core/actor_manage.h"

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

		virtual void loaddb_finish(pbdb::ENUM_DB atype, enum_dbstat astat);

		static void nregister();

		bool timer_handle([[maybe_unused]] const message<np_timerparm>& adata);

		template <typename T>
		bool handle([[maybe_unused]] const message<np_actor_forward<T, forward_c2g<forward>>>& adata)
		{
			// Client->Gate need tothis message toGameserver
			//adata.m_uid == socket id
			const auto* lpram = adata.get_data();
			const pack* lpack = adata.get_pack();
			gateway_socket* linfo = nullptr;
			if (sysconfig::robot_test())
			{
				i64_actorid lactorid = lpack->m_head->get_request_actor();
				nguid lguid(lactorid);
				linfo = m_info.get(lguid.area(), lguid.actordataid());
			}
			else
			{
				linfo = m_info.get(lpack->m_id);//m_info.get(adata.m_uid[0]);
			}
			if (linfo == nullptr)
			{
				log_error()->print("actor_gateway_c2g fail info == nullptr");
				return false;
			}

			ntcp::instance().send_server<forward, T>(
				linfo->m_gameid
				, lpram->m_data.m_data
				, nguid::make(ACTOR_ROLE, linfo->m_area, linfo->m_dataid)
				, lpack->m_head->get_request_actor()
			);
			return true;
		}

		bool handle([[maybe_unused]] const message<np_actor_gatewayinfo_updata>& adata);
	};
}//namespace ngl
