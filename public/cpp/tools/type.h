/*
* Copyright (c) [2020-2025] NingLeixueR
* 
* 项目名称：ngl_server
* 项目地址：https://github.com/NingLeixueR/ngl_server
* 
* 本文件是 ngl_server 项目的一部分，遵循 MIT 开源协议发布。
* 您可以按照协议规定自由使用、修改和分发本项目，包括商业用途，
* 但需保留原始版权和许可声明。
* 
* 许可详情参见项目根目录下的 LICENSE 文件：
* https://github.com/NingLeixueR/ngl_server/blob/main/LICENSE
*/
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