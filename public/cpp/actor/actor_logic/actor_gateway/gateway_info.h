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
// File overview: Declares interfaces for actor gateway.

#pragma once

#include "actor/protocol/nprotocol.h"
#include "actor/actor_base/nguid.h"

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

		// # Connection
		void remove_socket(i32_socket asocket);

		// # Deleteconnectioninfo
		void remove_actorid(i64_actorid aactorid);

		// # Area dataidgetgateway_socket
		gateway_socket* get(i16_area aarea, i32_actordataid aroleid);

		// # Socketgetgateway_socket
		gateway_socket* get(i32_socket asocket);

		// # Actoridgetgatewayid
		int64_t gatewayid(i64_actorid aid);

		// # Allsocket
		void foreach(const std::function<void(gateway_socket*)>& afun);

		// # Data
		void updata(const np_actor_gatewayinfo_updata& adata);
	};
}//namespace ngl