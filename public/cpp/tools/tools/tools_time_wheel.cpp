/*
* Copyright (c) [2020-2025] NingLeixueR
*
* Project: ngl_server
* License: MIT
*/
// File overview: Implements logic for tools.


#include "tools/tools/tools_time_wheel.h"
#include "tools/tools/tools_thread.h"

#include <unordered_map>
#include <algorithm>
#include <utility>
#include <cstdint>
#include <chrono>
#include <limits>
#include <memory>

namespace ngl::tools
{
	class wheel
	{
		friend class time_wheel;
		friend struct time_wheel::impl_time_wheel;

		wheel(const wheel&) = delete;
		wheel& operator=(const wheel&) = delete;

		std::vector<wheel_node*> m_slots;
		wheel* m_nextround = nullptr;
		wheel* m_lastround = nullptr;
		time_wheel* m_time_wheel = nullptr;

		// Slot size and slot count define how much wall-clock time this wheel
		// round covers.
		int64_t m_slot_ms = 0;
		int32_t m_slot_count = 0;
		int32_t m_slot_less = 0;
		int64_t m_slot_sum_ms = 0;
		int32_t m_current_pos = 0;
	public:
		wheel();
		inline void set(int64_t aslotms, int32_t aslotbit, wheel* anextround, wheel* alastround, time_wheel* atime_wheel);
		inline int slot_count();
		inline int slot_less();
		inline int64_t all_slot_ms();
		inline bool push_slots(wheel_node* anode);
		inline bool push(wheel_node* anode);
		inline wheel_node* shift_current_pos(int apos, wheel* awheel);
		inline wheel_node* shift_current_pos(wheel* awheel);
	};

	struct time_wheel::impl_time_wheel
	{
		time_wheel_config m_config;
		int64_t m_server_start_ms = 0;
		int64_t m_current_ms = 0;
		std::vector<std::shared_ptr<wheel>> m_wheel;
		std::unique_ptr<std::jthread> m_thread = nullptr;
		std::shared_mutex m_mutex;
		bool m_isthreadcallback = false;
		std::unique_ptr<std::jthread> m_threadcallback = nullptr;
		std::shared_mutex m_mutexcallback;
		ngl::tools::sem m_sem;
		// Ready callbacks live in a separate queue so the wheel thread keeps
		// advancing even when user handlers are slow.
		wheel_node* m_worldnodehead = nullptr;
		wheel_node* m_worldnodetail = nullptr;
		int64_t m_timerid = 1;
		std::unordered_map<int64_t, std::shared_ptr<std::atomic_bool>> m_timer;
		time_wheel* m_twheel = nullptr;
		std::atomic_bool m_stop = false;

		impl_time_wheel(time_wheel* atwheel, const time_wheel_config& aconfig, bool aisthreadcallback) :
			m_config(aconfig),
			m_isthreadcallback(aisthreadcallback),
			m_twheel(atwheel),
			m_stop(false)
		{
			if (m_config.m_time_wheel_precision <= 0)
			{
				m_config.m_time_wheel_precision = 1;
			}
			if (m_config.m_time_wheel_bit < 0)
			{
				m_config.m_time_wheel_bit = 0;
			}
			else if (m_config.m_time_wheel_bit > 30)
			{
				m_config.m_time_wheel_bit = 30;
			}
			if (m_config.m_time_wheel_count <= 0)
			{
				m_config.m_time_wheel_count = 1;
			}

			m_server_start_ms = getms();
			m_current_ms = m_server_start_ms;

			int64_t lms = m_config.m_time_wheel_precision;
			m_wheel.resize(m_config.m_time_wheel_count);
			for (int32_t i = 0; i < m_config.m_time_wheel_count; ++i)
			{
				m_wheel[i] = std::make_shared<wheel>();
			}
			for (int32_t i = 0; i < m_config.m_time_wheel_count; ++i)
			{
				m_wheel[i]->set(
					lms,
					m_config.m_time_wheel_bit,
					(i + 1 < m_config.m_time_wheel_count) ? m_wheel[i + 1].get() : nullptr,
					(i == 0) ? nullptr : m_wheel[i - 1].get(),
					atwheel
				);
				lms = m_wheel[i]->all_slot_ms();
			}

			m_thread = std::make_unique<std::jthread>(&impl_time_wheel::run, this);
			if (m_isthreadcallback)
			{
				m_threadcallback = std::make_unique<std::jthread>(&impl_time_wheel::runcallback, this);
			}
		}

		~impl_time_wheel()
		{
			stop();
			m_threadcallback.reset();
			m_thread.reset();
			clear_callback_nodes();
			{
				lock_write(m_mutex);
				for (auto& item : m_wheel)
				{
					if (item == nullptr)
					{
						continue;
					}
					for (wheel_node*& slot : item->m_slots)
					{
						remove(slot);
						slot = nullptr;
					}
				}
				m_wheel.clear();
				m_timer.clear();
			}
		}

		static wheel_node* callback_tail(wheel_node* ahead)
		{
			wheel_node* tail = ahead;
			while (tail != nullptr && tail->m_next != nullptr)
			{
				tail = tail->m_next;
			}
			return tail;
		}

		wheel_node* detach_callback_nodes()
		{
			wheel_node* head = nullptr;
			{
				lock_write(m_mutexcallback);
				std::swap(head, m_worldnodehead);
				m_worldnodetail = nullptr;
			}
			return head;
		}

		void append_callback_nodes(wheel_node* ahead, bool anotify_worker)
		{
			if (ahead == nullptr)
			{
				return;
			}

			lock_write(m_mutexcallback);
			if (m_worldnodehead == nullptr)
			{
				m_worldnodehead = ahead;
			}
			else
			{
				m_worldnodetail->m_next = ahead;
			}
			m_worldnodetail = callback_tail(ahead);
			if (anotify_worker)
			{
				m_sem.post();
			}
		}

		void clear_callback_nodes()
		{
			wheel_node* callback_head = detach_callback_nodes();
			while (callback_head != nullptr)
			{
				wheel_node* next_node = callback_head->m_next;
				std::unique_ptr<wheel_node> node_owner(callback_head);
				callback_head = next_node;
			}
		}

		void remove(wheel_node* anode)
		{
			wheel_node* next_node = nullptr;
			for (wheel_node* node = anode; node != nullptr; node = next_node)
			{
				std::unique_ptr<wheel_node> node_owner(node);
				next_node = node->m_next;
				m_timer.erase(node->m_timerid);
			}
		}

		int64_t all_slot_ms() const
		{
			return m_wheel[m_config.m_time_wheel_count - 1]->all_slot_ms();
		}

		int64_t duration_ms() const
		{
			return m_current_ms - m_server_start_ms;
		}

		bool schedule_locked(wheel_node* apnode)
		{
			wheel_node* next_node = nullptr;
			wheel_node* expired_nodes = nullptr;
			const int64_t elapsed_ms = duration_ms();
			const int64_t wheel_span_ms = all_slot_ms();
			bool scheduled = false;

			for (wheel_node* node = apnode; node != nullptr; node = next_node)
			{
				next_node = node->m_next;
				node->m_next = nullptr;

				if (node->m_parm.m_ms > wheel_span_ms)
				{
					node->m_next = expired_nodes;
					expired_nodes = node;
					continue;
				}

				if (node->m_parm.m_ms < elapsed_ms)
				{
					// Already-due timers are delivered immediately. Repeating
					// timers clone the next instance before the current callback
					// is queued.
					const int interval_ms = node->m_parm.m_intervalms != nullptr
						? node->m_parm.m_intervalms(m_current_ms)
						: 0;
					const bool should_repeat = interval_ms > 0 && node->m_parm.m_count - 1 > 0;
					if (should_repeat)
					{
						auto callback_copy = std::make_unique<wheel_node>(*node);
						push(callback_copy.release());
						--node->m_parm.m_count;
						node->m_parm.m_ms += interval_ms;
						scheduled = schedule_locked(node) || scheduled;
					}
					else
					{
						push(node);
						scheduled = true;
					}
					continue;
				}

				for (auto& wheel_round : m_wheel)
				{
					// Push into the first round whose coverage is large enough for
					// the timer's target time.
					if (!wheel_round->push(node))
					{
						continue;
					}
					m_timer.insert_or_assign(node->m_timerid, node->m_remove);
					scheduled = true;
					node = nullptr;
					break;
				}

				if (node != nullptr)
				{
					node->m_next = expired_nodes;
					expired_nodes = node;
				}
			}

			if (expired_nodes != nullptr)
			{
				remove(expired_nodes);
			}
			return scheduled;
		}

		void removetimer(int64_t atimerid)
		{
			lock_write(m_mutex);
			auto itor = m_timer.find(atimerid);
			if (itor == m_timer.end() || itor->second == nullptr)
			{
				return;
			}
			itor->second->store(true, std::memory_order_relaxed);
		}

		void push(wheel_node* apnode)
		{
			// In threaded mode the timer thread wakes the dedicated callback
			// worker; otherwise manual consumers drain the same queue through
			// pop_node().
			append_callback_nodes(apnode, m_isthreadcallback);
		}

		void stop()
		{
			m_stop.store(true, std::memory_order_relaxed);
			m_sem.post();
		}

		void run()
		{
			const int32_t tick_precision_ms = m_config.m_time_wheel_precision;
			while (!m_stop.load(std::memory_order_relaxed))
			{
				const int64_t elapsed_since_tick_ms = getms() - m_current_ms;
				const int64_t sleep_ms = tick_precision_ms - elapsed_since_tick_ms;
				if (sleep_ms > 0)
				{
					std::this_thread::sleep_for(std::chrono::milliseconds(sleep_ms));
				}

				lock_write(m_mutex);
				if (m_wheel.empty())
				{
					continue;
				}

				wheel_node* rescheduled_nodes = m_wheel[0]->shift_current_pos(nullptr);
				if (rescheduled_nodes != nullptr)
				{
					// Periodic timers returned by the current slot are scheduled
					// again on the same absolute timeline.
					schedule_locked(rescheduled_nodes);
				}
				m_current_ms += tick_precision_ms;
			}
		}

		void runcallback()
		{
			while (true)
			{
				m_sem.wait();

				wheel_node* callback_head = detach_callback_nodes();

				// Execute outside the queue lock so callbacks can schedule or
				// cancel other timers freely.
				wheel_node* next_node = nullptr;
				for (wheel_node* node = callback_head; node != nullptr; node = next_node)
				{
					next_node = node->m_next;
					std::unique_ptr<wheel_node> node_owner(node);
					if (!node->removed() && node->m_parm.m_fun != nullptr)
					{
						node->m_parm.m_fun(node);
					}
				}

				if (m_stop.load(std::memory_order_relaxed))
				{
					wheel_node* lremaining = nullptr;
					{
						lock_write(m_mutexcallback);
						lremaining = m_worldnodehead;
					}
					if (lremaining == nullptr)
					{
						return;
					}
				}
			}
		}

		std::shared_ptr<wheel_node> pop_node()
		{
			for (;;)
			{
				wheel_node* head_node = nullptr;
				{
					lock_write(m_mutexcallback);
					if (m_worldnodehead == nullptr)
					{
						return nullptr;
					}
					if (m_worldnodehead == m_worldnodetail)
					{
						head_node = m_worldnodehead;
						m_worldnodehead = nullptr;
						m_worldnodetail = nullptr;
					}
					else
					{
						head_node = m_worldnodehead;
						m_worldnodehead = m_worldnodehead->m_next;
						head_node->m_next = nullptr;
					}
				}

				if (head_node != nullptr && head_node->removed())
				{
					std::unique_ptr<wheel_node> node_owner(head_node);
					continue;
				}

				return std::shared_ptr<wheel_node>(head_node);
			}
		}

		int64_t addtimer(const wheel_parm& apram)
		{
			wheel_parm lparm = apram;
			const int64_t lnow = getms();
			lparm.m_timerstart = lnow;
			// Convert the relative delay into the absolute timeline tracked by
			// the hierarchical wheel.
			lparm.m_ms += static_cast<int32_t>(lnow - m_server_start_ms);
			if (lparm.m_ms < 0)
			{
				return -1;
			}
			if (lparm.m_count == -1)
			{
				lparm.m_count = std::numeric_limits<int>::max();
			}

			lock_write(m_mutex);
			const int64_t timer_id = ++m_timerid;
			auto removed_flag = std::make_shared<std::atomic_bool>(false);
			auto node = std::make_unique<wheel_node>(m_twheel, timer_id, removed_flag, lparm);
			m_timer.insert_or_assign(timer_id, removed_flag);
			if (!schedule_locked(node.get()))
			{
				m_timer.erase(timer_id);
				return -1;
			}
			node.release();
			return timer_id;
		}
	};

	wheel_node::wheel_node(time_wheel* atw, int64_t atimerid, const std::shared_ptr<std::atomic_bool>& aremove, const wheel_parm& aparm) :
		m_tw(atw),
		m_timerid(atimerid),
		m_remove(aremove),
		m_next(nullptr),
		m_parm(aparm)
	{
	}

	wheel_node::wheel_node(const wheel_node& aparm) :
		m_tw(aparm.m_tw),
		m_timerid(aparm.m_timerid),
		m_remove(aparm.m_remove),
		m_next(nullptr),
		m_parm(aparm.m_parm)
	{
	}

	wheel::wheel() :
		m_nextround(nullptr),
		m_lastround(nullptr),
		m_time_wheel(nullptr),
		m_slot_ms(0),
		m_slot_count(0),
		m_slot_less(0),
		m_slot_sum_ms(0),
		m_current_pos(0)
	{
	}

	void wheel::set(int64_t aslotms, int32_t aslotbit, wheel* anextround, wheel* alastround, time_wheel* atime_wheel)
	{
		m_nextround = anextround;
		m_slot_ms = aslotms;
		m_time_wheel = atime_wheel;
		if (aslotbit < 0 || aslotbit > 30)
		{
			aslotbit = 0;
		}
		m_slot_count = static_cast<int32_t>(std::uint32_t{ 1 } << static_cast<std::uint32_t>(aslotbit));
		m_slot_less = m_slot_count - 1;
		m_current_pos = alastround != nullptr ? 0 : -1;
		m_slot_sum_ms = m_slot_ms * m_slot_count;
		m_lastround = alastround;
		m_slots.resize(m_slot_count);
	}

	int wheel::slot_count()
	{
		return m_slot_count;
	}

	int wheel::slot_less()
	{
		return m_slot_less;
	}

	int64_t wheel::all_slot_ms()
	{
		return m_slot_sum_ms;
	}

	bool wheel::push_slots(wheel_node* anode)
	{
		// Slot selection uses the absolute due time. If the slot is already
		// behind the current cursor, the node is pushed down to the previous
		// round to preserve timing precision.
		int slot_index = static_cast<int32_t>(anode->m_parm.m_ms / m_slot_ms);
		slot_index &= m_slot_less;
		const int current_slot_index = m_current_pos & m_slot_less;
		if (slot_index <= current_slot_index && m_current_pos != -1)
		{
			if (m_lastround != nullptr)
			{
				m_lastround->push_slots(anode);
				return true;
			}
			if (slot_index != current_slot_index)
			{
				return true;
			}
		}
		anode->m_next = m_slots[slot_index];
		m_slots[slot_index] = anode;
		return true;
	}

	bool wheel::push(wheel_node* anode)
	{
		if (anode->m_parm.m_ms >= m_slot_sum_ms)
		{
			return false;
		}
		return push_slots(anode);
	}

	wheel_node* wheel::shift_current_pos(int apos, wheel* awheel)
	{
		auto& impl = m_time_wheel->m_impl_time_wheel();
		for (wheel_node* node = m_slots[apos], *next_node = nullptr; node != nullptr; node = next_node)
		{
			next_node = node->m_next;
			node->m_next = nullptr;
			if (awheel->push_slots(node))
			{
				continue;
			}
			impl->remove(node);
		}
		m_slots[apos] = nullptr;
		return nullptr;
	}

	wheel_node* wheel::shift_current_pos(wheel* awheel)
	{
		auto& impl = m_time_wheel->m_impl_time_wheel();
		++m_current_pos;
		const int slot_index = m_current_pos & m_slot_less;
		if (slot_index == 0 && m_current_pos != 0)
		{
			if (m_nextround == nullptr)
			{
				return nullptr;
			}
			// When the round wraps, pull one bucket from the next round back
			// into this round before consuming the current slot.
			m_nextround->shift_current_pos(this);
			if (awheel != nullptr)
			{
				return shift_current_pos(slot_index, awheel);
			}
		}
		else if (awheel != nullptr)
		{
			return shift_current_pos(slot_index, awheel);
		}

		wheel_node* rescheduled_nodes = nullptr;
		for (wheel_node* node = m_slots[slot_index], *next_node = nullptr; node != nullptr; node = next_node)
		{
			next_node = node->m_next;
			node->m_next = nullptr;
			if (node->removed())
			{
				impl->remove(node);
				continue;
			}

			if (node->m_parm.m_intervalms == nullptr)
			{
				impl->push(node);
				continue;
			}

			const int interval_ms = node->m_parm.m_intervalms(impl->m_current_ms);
			--node->m_parm.m_count;
			if (interval_ms <= 0 || node->m_parm.m_count <= 0)
			{
				impl->push(node);
				continue;
			}

			// Queue the callback node now and keep a fresh copy for the next interval.
			auto callback_copy = std::make_unique<wheel_node>(*node);
			impl->push(callback_copy.release());
			node->m_parm.m_ms += interval_ms;
			node->m_next = rescheduled_nodes;
			rescheduled_nodes = node;
		}
		m_slots[slot_index] = nullptr;
		return rescheduled_nodes;
	}

	int64_t time_wheel::getms()
	{
		return std::chrono::duration_cast<std::chrono::milliseconds>(
			std::chrono::system_clock::now().time_since_epoch()
		).count();
	}

	time_wheel::time_wheel(const time_wheel_config& aconfig, bool aisthreadcallback)
	{
		m_impl_time_wheel.make_unique(this, aconfig, aisthreadcallback);
	}

	time_wheel::~time_wheel() = default;

	int time_wheel::count()
	{
		lock_read(m_impl_time_wheel()->m_mutex);
		return static_cast<int>(m_impl_time_wheel()->m_timer.size());
	}

	bool time_wheel::empty()
	{
		lock_read(m_impl_time_wheel()->m_mutex);
		return m_impl_time_wheel()->m_timer.empty();
	}

	int64_t time_wheel::server_start_ms()
	{
		lock_read(m_impl_time_wheel()->m_mutex);
		return m_impl_time_wheel()->m_server_start_ms;
	}

	int64_t time_wheel::server_current_ms()
	{
		lock_read(m_impl_time_wheel()->m_mutex);
		return m_impl_time_wheel()->m_current_ms;
	}

	int64_t time_wheel::addtimer(const wheel_parm& apram)
	{
		return m_impl_time_wheel()->addtimer(apram);
	}

	void time_wheel::removetimer(int64_t atimerid)
	{
		m_impl_time_wheel()->removetimer(atimerid);
	}

	std::shared_ptr<wheel_node> time_wheel::pop_node()
	{
		return m_impl_time_wheel()->pop_node();
	}

	time_wheel twheel::m_wheel;
}// namespace ngl
