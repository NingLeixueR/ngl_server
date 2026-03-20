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
// File overview: Declares helpers for socket session id ranges.

#pragma once

#include "actor/tab/csvtable.h"
#include "tools/type.h"

#include <limits>

namespace ngl::net_session
{
	enum : i32_sessionid
	{
		session_begin_tcp = 1,
		session_begin_ws = 0x10000000,
	};

	constexpr i32_sessionid begin(ENET_PROTOCOL aprotocol)
	{
		switch (aprotocol)
		{
		case ENET_TCP:
			return session_begin_tcp;
		case ENET_WS:
			return session_begin_ws;
		default:
			return 0;
		}
	}

	constexpr i32_sessionid end(ENET_PROTOCOL aprotocol)
	{
		switch (aprotocol)
		{
		case ENET_TCP:
			return session_begin_ws - 1;
		case ENET_WS:
			return std::numeric_limits<i32_sessionid>::max();
		default:
			return 0;
		}
	}

	inline bool next(i32_sessionid& asessionid, ENET_PROTOCOL aprotocol)
	{
		const i32_sessionid lbegin = begin(aprotocol);
		const i32_sessionid lend = end(aprotocol);
		if (lbegin <= 0 || lend < lbegin)
		{
			return false;
		}
		if (asessionid < lbegin - 1)
		{
			asessionid = lbegin - 1;
		}
		if (asessionid >= lend)
		{
			return false;
		}
		++asessionid;
		return true;
	}

	constexpr ENET_PROTOCOL protocol(i32_sessionid asessionid)
	{
		if (asessionid >= session_begin_ws)
		{
			return ENET_WS;
		}
		if (asessionid >= session_begin_tcp)
		{
			return ENET_TCP;
		}
		return ENET_NULL;
	}
}// namespace ngl::net_session
