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
// File overview: Implements logic for protocol.


#include "actor/protocol/tprotocol.h"
#include "actor/protocol/nprotocol.h"
#include "actor/actor_base/core/ntimer.h"
#include "actor/actor_base/core/actor.h"
#include "actor/generated/pb/net.pb.h"

namespace ngl
{
	std::map<tools::nhashcode, tprotocol::info> tprotocol::m_keyval;
	std::map<i32_protocolnum, tprotocol::info*> tprotocol::m_protocol;
	std::map<std::string, tprotocol::info*> tprotocol::m_nameprotocol;
	int32_t tprotocol::m_customs = 200000000;
}//namespace ngl