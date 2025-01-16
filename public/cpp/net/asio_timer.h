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
		static time_wheel& wheel()
		{
			return m_wheel;
		}
	};
}// namespace ngl