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
// File overview: Declares interfaces for tools.

#pragma once

#include "tools/time_wheel.h"
#include "tools/log/nlog.h"

namespace ngl
{
	class nconsume
	{
		nconsume() = delete;
		nconsume(const nconsume&) = delete;
		nconsume& operator=(const nconsume&) = delete;

		std::string		m_name;
		int64_t			m_beg;
	public:
		nconsume(const std::string& aname);
		void start();
		void finish();
	};
}//namespace ngl