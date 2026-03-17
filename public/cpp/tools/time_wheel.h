/*
* Copyright (c) [2020-2025] NingLeixueR
*
* Project: ngl_server
* License: MIT
*/
// File overview: Declares interfaces for tools.

#pragma once

#include "tools/impl.h"
#include "tools/threadtools.h"

#include <array>
#include <atomic>
#include <cmath>
#include <cstdint>
#include <functional>
#include <map>
#include <memory>
#include <mutex>
#include <thread>
#include <vector>

namespace ngl
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
			int64_t lsum = 0;
			int64_t lprecision = m_time_wheel_precision;
			for (int i = 0; i < m_time_wheel_count; ++i)
			{
				lprecision *= static_cast<int64_t>(std::pow(2, m_time_wheel_bit));
				lsum += lprecision;
			}
			return lsum;
		}

		double year()
		{
			return day() / 365.0;
		}

		double day()
		{
			const double lnum = 24 * 60 * 60 * 1000;
			const uint64_t lsum = max_time();
			return lsum / lnum;
		}
	};

	class time_wheel
	{
		friend class wheel;

		time_wheel(const time_wheel&) = delete;
		time_wheel& operator=(const time_wheel&) = delete;

		struct impl_time_wheel;
		ngl::impl<impl_time_wheel> m_impl_time_wheel;
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
