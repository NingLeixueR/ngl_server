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
// File overview: Implements logic for net.


#include "actor/protocol/nprotocol.h"
#include "net/asio_timer.h"
#include "tools/log/nlog.h"

namespace ngl
{
	tools::time_wheel asio_timer::m_wheel(
		tools::time_wheel_config
		{ 
			.m_time_wheel_precision = 1000, // 1 ms precision bucket size.
			.m_time_wheel_bit = 8,          // 256 slots per wheel level.
			.m_time_wheel_count = 4         // Four cascading levels for a wide timer range.
		}
	);

	tools::time_wheel& asio_timer::wheel()
	{
		return m_wheel;
	}
}// namespace ngl
