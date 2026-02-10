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

#include "time_wheel.h"

namespace ngl
{
	// # 服务于网络不急迫事件  秒级别定时器
	class asio_timer
	{
		asio_timer(const asio_timer&) = delete;
		asio_timer& operator=(const asio_timer&) = delete;

		asio_timer() {}

		static time_wheel m_wheel;
	public:
		static time_wheel& wheel();
	};
}// namespace ngl