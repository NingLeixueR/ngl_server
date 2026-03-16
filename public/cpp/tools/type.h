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

#include <cstdint>
#include <string>
#include <memory>

namespace ngl
{
	using i32_serverid		= int32_t;
	using i32_packid		= int32_t;
	using i32_gatewayid		= i32_serverid;
	using i32_sessionid		= int32_t;
	using i32_socket		= i32_sessionid;
	using i32_time			= int32_t;
	using i64_actorid		= int64_t;
	using i16_actortype		= int16_t;
	using i16_area			= int16_t;
	using i32_actordataid	= int32_t;
	using i32_threadid		= int32_t;
	using i32_protocolnum	= int32_t;
	using i16_port			= uint16_t;
	using i32_threadsize	= int32_t;
	using i32_id			= int32_t;
	using i32_line			= int32_t;
	using i64_accountid		= int64_t;
	using i32_threadnumber	= int32_t;
	using i32_session		= int32_t;
	using i8_sesindex		= int8_t;
	using i32_protocol		= int32_t;
	using str_ip			= std::string;
	using str_host			= std::string;
	using str_hostport		= std::string;
	using str_servername	= std::string;

	using i32_taskid		= int32_t;
	using i32_rolelv		= int32_t;
	using i32_rolevip		= int32_t;

	using i64_startfinishtime = int64_t;

	using i64_hashcode		= size_t;

	// nsp
	using i64_dataid		= int64_t;
	using i64_nodeid		= int64_t;
	using i32_fieldnumber	= int32_t;
}//namespace ngl