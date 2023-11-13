#include "asio_timer.h"
#include "nlog.h"

namespace ngl
{
	time_wheel asio_timer::m_wheel(
		time_wheel_config
		{ 
			.m_time_wheel_precision = 1000,
			.m_time_wheel_bit = 8,
			.m_time_wheel_count = 4
		}
	);
}
