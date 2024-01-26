#pragma once

#include "segpack.h"
#include "type.h"
#include "pack.h"
#include "threadtools.h"
#include "impl.h"

#include <map>
#include <functional>

#define DEF_TIMEOUT_SECOND (600000)

namespace ngl
{
	enum enum_segpack
	{
		rate_interval		= 1,			// 速率检测的间隔(单位秒)
		rate_size			= 20,			// 速率 每秒多少条
		heart_beat_interval = 10,			// 心跳间隔(单位秒)
	};

	struct segpack_rate_interval
	{
		int m_resetutc;		// 上次重置的utc
		int m_ratecount;	// 收到数据包的数量

		segpack_rate_interval();
		void reset();
	};

	class segpack_rate
	{
		std::map<i32_socket, segpack_rate_interval> m_data;
	public:
		static int m_max_rate_count;
		static int m_rate_interval;

		bool add(i32_socket aid);
	};

	class segpack_heartbeat
	{
	public:
		static bool is_heartbeat(int aprotocolnum);
	};

	class segpack
	{
		struct impl_segpack;
		ngl::impl<impl_segpack> m_impl_segpack;

	public:
		segpack();

		bool push(i32_socket aid, const char* ap, int alen, bool aislanip);
		void close(i32_socket aid);
	};
}