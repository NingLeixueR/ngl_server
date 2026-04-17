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
// File overview: Gateway session info tracking connected clients and their routing metadata.

#pragma once

#include "actor/protocol/nprotocol.h"
#include "actor/actor_base/core/nguid.h"

#include <map>

namespace ngl
{
	class gateway_info
	{
		gateway_info(const gateway_info&) = delete;
		gateway_info& operator=(const gateway_info&) = delete;

		std::map<i16_area, std::map<i32_actordataid, gateway_socket>>	m_info;		// key:area  value.key:roleid
		std::map<i32_socket, gateway_socket*>							m_sockinfo;	// key:socket
	public:
		gateway_info() = default;

		void updata(const gateway_socket& ainfo);

		bool updata_socket(i16_area aarea, i32_actordataid aactordataid, i32_socket asocket);

		std::map<i16_area, std::map<i32_actordataid, gateway_socket>>& info();

		// Remove a client connection by socket id.
		void remove_socket(i32_socket asocket);

		// Remove a client connection by actor id.
		void remove_actorid(i64_actorid aactorid);

		// Look up a gateway_socket by area and role id.
		gateway_socket* get(i16_area aarea, i32_actordataid aroleid);

		// Look up a gateway_socket by socket id.
		gateway_socket* get(i32_socket asocket);

		// Get the gateway id for a given actor id.
		int64_t gatewayid(i64_actorid aid);

		// Iterate over all connected client sockets.
		void foreach(const std::function<void(gateway_socket*)>& afun);

		// Update gateway info from a sync message.
		void updata(const np_actor_gatewayinfo_updata& adata);
	};
}//namespace ngl