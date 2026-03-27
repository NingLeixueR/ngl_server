/*
* Copyright (c) [2020-2025] NingLeixueR
*
* Project: ngl_server
* License: MIT
*/
// File overview: Declares interfaces for tools.

#pragma once

#include "tools/tools/tools_thread.h"
#include "tools/tools/tools_impl.h"

#include <functional>
#include <algorithm>
#include <cstdint>
#include <limits>
#include <atomic>
#include <memory>
#include <thread>
#include <vector>
#include <array>
#include <mutex>
#include <map>

namespace ngl::tools
{
	struct wheel_node;
	class time_wheel;
	class wheel;

	struct wheel_parm
	{
		// `m_ms` becomes an absolute due time relative to server start once the
		// timer is inserted into the wheel.
		int64_t m_timerstart = 0;
		int32_t m_ms = 0;
		// Returning a positive interval reschedules the timer. Returning <= 0
		// stops repeat scheduling after the current firing.
		std::function<int(int64_t)> m_intervalms = nullptr;
		int m_count = 0;
		void* m_pram = nullptr;
		std::function<void(const wheel_node*)> m_fun = nullptr;
	};

	struct wheel_node
	{
		using timecallback = std::function<void(wheel_node*)>;

		time_wheel* m_tw = nullptr;
		int64_t m_timerid = 0;
		// Repeated timers share this flag across copies so one remove operation
		// cancels every future firing.
		std::shared_ptr<std::atomic_bool> m_remove;
		wheel_node* m_next = nullptr;
		wheel_parm m_parm;

		wheel_node(time_wheel* atw, int64_t atimerid, const std::shared_ptr<std::atomic_bool>& aremove, const wheel_parm& aparm);
		wheel_node(const wheel_node& aparm);
		wheel_node() = delete;

		bool removed() const
		{
			return m_remove != nullptr && m_remove->load(std::memory_order_relaxed);
		}
	};

	struct time_wheel_config
	{
	public:
		// The wheel is hierarchical: each round contains 2^bit slots, and every
		// higher round covers the full time range of the round below it.
		int32_t m_time_wheel_precision = 10;
		int32_t m_time_wheel_bit = 8;
		int32_t m_time_wheel_count = 4;

		int64_t max_time()
		{
			const int32_t precision_ms = std::max(m_time_wheel_precision, 0);
			const int32_t wheel_bit = std::clamp(m_time_wheel_bit, 0, 30);
			const int32_t wheel_count = std::max(m_time_wheel_count, 0);
			const int64_t round_scale = int64_t{ 1 } << static_cast<std::uint32_t>(wheel_bit);
			int64_t total_ms = 0;
			int64_t round_ms = precision_ms;
			for (int32_t index = 0; index < wheel_count; ++index)
			{
				if (round_ms > std::numeric_limits<int64_t>::max() / round_scale)
				{
					return std::numeric_limits<int64_t>::max();
				}
				round_ms *= round_scale;
				if (total_ms > std::numeric_limits<int64_t>::max() - round_ms)
				{
					return std::numeric_limits<int64_t>::max();
				}
				total_ms += round_ms;
			}
			return total_ms;
		}

		double year()
		{
			return day() / 365.0;
		}

		double day()
		{
			const double milliseconds_per_day = 24.0 * 60.0 * 60.0 * 1000.0;
			return static_cast<double>(max_time()) / milliseconds_per_day;
		}
	};

	class time_wheel
	{
		friend class wheel;

		time_wheel(const time_wheel&) = delete;
		time_wheel& operator=(const time_wheel&) = delete;

		struct impl_time_wheel;
		tools::impl<impl_time_wheel> m_impl_time_wheel;
	public:
		static int64_t getms();
		time_wheel(const time_wheel_config& aconfig = time_wheel_config(), bool aisthreadcallback = true);
		~time_wheel();

		int count();
		bool empty();
		int64_t server_start_ms();
		int64_t server_current_ms();
		// Returns a stable timer id. `m_count == -1` means "repeat forever".
		int64_t addtimer(const wheel_parm& apram);
		// Removal is lazy and marks the shared cancel flag instead of scanning
		// every wheel slot.
		void removetimer(int64_t atimerid);
		// Used when callbacks are consumed manually instead of from the
		// dedicated callback thread.
		std::shared_ptr<wheel_node> pop_node();
	};

	class twheel
	{
		twheel() = delete;
		// Shared default timer wheel used by subsystems that do not need a
		// dedicated scheduler instance.
		static time_wheel m_wheel;
	public:
		static time_wheel& wheel()
		{
			return m_wheel;
		}
	};
}// namespace ngl
