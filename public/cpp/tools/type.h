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
}