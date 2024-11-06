#pragma once

#include "threadtools.h"
#include "segpack.h"
#include "type.h"
#include "pack.h"
#include "impl.h"

#include <functional>
#include <map>

namespace ngl
{
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
		bool add(i32_socket aid);
	};

	class segpack_heartbeat
	{
	public:
		static bool is_heartbeat(i32_protocolnum aprotocolnum);
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
}// namespace ngl