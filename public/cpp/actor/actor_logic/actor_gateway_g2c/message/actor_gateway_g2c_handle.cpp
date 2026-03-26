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
// File overview: Implements message handlers for message.


#include "actor/actor_logic/actor_gateway_g2c/actor_gateway_g2c.h"
#include "actor/actor_base/core/nregister.h"
#include "actor/actor_logic/nforward.h"

namespace ngl
{
	bool actor_gateway_g2c::handle(const message<np_actor_gatewayinfo_updata>& adata)
	{
		m_info.updata(*adata.get_data());
		return true;
	}
}//namespace ngl
