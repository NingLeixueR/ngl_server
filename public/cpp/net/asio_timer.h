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
// File overview: Declares interfaces for net.

#pragma once

#include "tools/time_wheel.h"

namespace ngl
{
	// # Event timer
	class asio_timer
	{
		asio_timer(const asio_timer&) = delete;
		asio_timer& operator=(const asio_timer&) = delete;
		asio_timer() = delete;

		static time_wheel m_wheel;
	public:
		static time_wheel& wheel();
	};
}// namespace ngl