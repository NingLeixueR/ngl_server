/*
* Copyright (c) [2020-2025] NingLeixueR
*
* Project: ngl_server
* License: MIT
*/
// File overview: Implements logic for tools.


#include "tools/threadtools.h"
#include "tools/time_wheel.h"

#include <algorithm>
#include <chrono>
#include <cstdint>
#include <limits>
#include <memory>
#include <unordered_map>
#include <utility>

namespace ngl
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
		ngl::sem m_sem;
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

		void clear_callback_nodes()
		{
			wheel_node* lhead = nullptr;
			{
				lock_write(m_mutexcallback);
				std::swap(lhead, m_worldnodehead);
				m_worldnodetail = nullptr;
			}
			while (lhead != nullptr)
			{
				wheel_node* lnext = lhead->m_next;
				delete lhead;
				lhead = lnext;
			}
		}

		void remove(wheel_node* anode)
		{
			wheel_node* lpnextnode = nullptr;
			for (wheel_node* lpnode = anode; lpnode != nullptr; lpnode = lpnextnode)
			{
				std::unique_ptr<wheel_node> ltemp(lpnode);
				lpnextnode = lpnode->m_next;
				m_timer.erase(lpnode->m_timerid);
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
			wheel_node* lpnextnode = nullptr;
			wheel_node* removenode = nullptr;
			const int64_t lduration = duration_ms();
			const int64_t lallslot = all_slot_ms();
			bool lscheduled = false;

			for (wheel_node* lpnode = apnode; lpnode != nullptr; lpnode = lpnextnode)
			{
				lpnextnode = lpnode->m_next;
				lpnode->m_next = nullptr;

				if (lpnode->m_parm.m_ms > lallslot)
				{
					lpnode->m_next = removenode;
					removenode = lpnode;
					continue;
				}

				if (lpnode->m_parm.m_ms < lduration)
				{
					const int lintervalms = lpnode->m_parm.m_intervalms != nullptr
						? lpnode->m_parm.m_intervalms(m_current_ms)
						: 0;
					const bool lcopy = lintervalms > 0 && lpnode->m_parm.m_count - 1 > 0;
					if (lcopy)
					{
						wheel_node* lpnewnode = new wheel_node(*lpnode);
						push(lpnewnode);
						--lpnode->m_parm.m_count;
						lpnode->m_parm.m_ms += lintervalms;
						lscheduled = schedule_locked(lpnode) || lscheduled;
					}
					else
					{
						push(lpnode);
						lscheduled = true;
					}
					continue;
				}

				for (auto& item : m_wheel)
				{
					if (!item->push(lpnode))
					{
						continue;
					}
					m_timer.insert_or_assign(lpnode->m_timerid, lpnode->m_remove);
					lscheduled = true;
					lpnode = nullptr;
					break;
				}

				if (lpnode != nullptr)
				{
					lpnode->m_next = removenode;
					removenode = lpnode;
				}
			}

			if (removenode != nullptr)
			{
				remove(removenode);
			}
			return lscheduled;
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
			if (apnode == nullptr)
			{
				return;
			}

			wheel_node* ltailnode = nullptr;
			if (m_isthreadcallback)
			{
				lock_write(m_mutexcallback);
				if (m_worldnodehead == nullptr)
				{
					m_worldnodehead = apnode;
					ltailnode = m_worldnodehead;
				}
				else
				{
					m_worldnodetail->m_next = apnode;
					ltailnode = m_worldnodetail;
				}
				while (ltailnode->m_next != nullptr)
				{
					ltailnode = ltailnode->m_next;
				}
				m_worldnodetail = ltailnode;
				m_sem.post();
				return;
			}

			lock_write(m_mutexcallback);
			if (m_worldnodehead == nullptr)
			{
				m_worldnodehead = apnode;
				ltailnode = m_worldnodehead;
			}
			else
			{
				m_worldnodetail->m_next = apnode;
				ltailnode = m_worldnodetail;
			}
			while (ltailnode->m_next != nullptr)
			{
				ltailnode = ltailnode->m_next;
			}
			m_worldnodetail = ltailnode;
		}

		void stop()
		{
			m_stop.store(true, std::memory_order_relaxed);
			m_sem.post();
		}

		void run()
		{
			while (!m_stop.load(std::memory_order_relaxed))
			{
				const int32_t ltemp = static_cast<int32_t>(getms() - m_current_ms);
				const int32_t ltempsleep = m_config.m_time_wheel_precision - ltemp;
				if (ltempsleep > 0)
				{
					std::this_thread::sleep_for(std::chrono::milliseconds(ltempsleep));
				}

				lock_write(m_mutex);
				if (m_wheel.empty())
				{
					continue;
				}

				wheel_node* lpbnode = m_wheel[0]->shift_current_pos(nullptr);
				if (lpbnode != nullptr)
				{
					schedule_locked(lpbnode);
				}
				m_current_ms += m_config.m_time_wheel_precision;
			}
		}

		void runcallback()
		{
			while (true)
			{
				m_sem.wait();

				wheel_node* lpnode = nullptr;
				{
					lock_write(m_mutexcallback);
					std::swap(lpnode, m_worldnodehead);
					m_worldnodetail = nullptr;
				}

				wheel_node* lnextnode = nullptr;
				for (wheel_node* pnode = lpnode; pnode != nullptr; pnode = lnextnode)
				{
					lnextnode = pnode->m_next;
					if (!pnode->removed() && pnode->m_parm.m_fun != nullptr)
					{
						pnode->m_parm.m_fun(pnode);
					}
					delete pnode;
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
			lock_write(m_mutexcallback);
			wheel_node* ret = nullptr;
			if (m_worldnodehead == nullptr)
			{
				return nullptr;
			}
			if (m_worldnodehead == m_worldnodetail)
			{
				ret = m_worldnodehead;
				m_worldnodehead = nullptr;
				m_worldnodetail = nullptr;
				return std::shared_ptr<wheel_node>(ret);
			}

			ret = m_worldnodehead;
			m_worldnodehead = m_worldnodehead->m_next;
			ret->m_next = nullptr;
			return std::shared_ptr<wheel_node>(ret);
		}

		int64_t addtimer(const wheel_parm& apram)
		{
			wheel_parm lparm = apram;
			const int64_t lnow = getms();
			lparm.m_timerstart = lnow;
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
			const int64_t ltimerid = ++m_timerid;
			auto lremove = std::make_shared<std::atomic_bool>(false);
			std::unique_ptr<wheel_node> lpnode(new wheel_node(m_twheel, ltimerid, lremove, lparm));
			m_timer.insert_or_assign(ltimerid, lremove);
			if (!schedule_locked(lpnode.get()))
			{
				m_timer.erase(ltimerid);
				return -1;
			}
			lpnode.release();
			return ltimerid;
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
		int lcallcount = static_cast<int32_t>(anode->m_parm.m_ms / m_slot_ms);
		lcallcount &= m_slot_less;
		const int lcount = m_current_pos & m_slot_less;
		if (lcallcount <= lcount && m_current_pos != -1)
		{
			if (m_lastround != nullptr)
			{
				m_lastround->push_slots(anode);
				return true;
			}
			if (lcallcount != lcount)
			{
				return true;
			}
		}
		anode->m_next = m_slots[lcallcount];
		m_slots[lcallcount] = anode;
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
		for (wheel_node* lpnode = m_slots[apos], *ltempnode = nullptr; lpnode != nullptr; lpnode = ltempnode)
		{
			ltempnode = lpnode->m_next;
			lpnode->m_next = nullptr;
			if (awheel->push_slots(lpnode))
			{
				continue;
			}
			m_time_wheel->m_impl_time_wheel()->remove(lpnode);
		}
		m_slots[apos] = nullptr;
		return nullptr;
	}

	wheel_node* wheel::shift_current_pos(wheel* awheel)
	{
		++m_current_pos;
		const int lpos = m_current_pos & m_slot_less;
		if (lpos == 0 && m_current_pos != 0)
		{
			if (m_nextround == nullptr)
			{
				return nullptr;
			}
			m_nextround->shift_current_pos(this);
			if (awheel != nullptr)
			{
				return shift_current_pos(lpos, awheel);
			}
		}
		else if (awheel != nullptr)
		{
			return shift_current_pos(lpos, awheel);
		}

		wheel_node* lpushnode = nullptr;
		for (wheel_node* lpnode = m_slots[lpos], *ltempnode = nullptr; lpnode != nullptr; lpnode = ltempnode)
		{
			ltempnode = lpnode->m_next;
			lpnode->m_next = nullptr;
			if (!lpnode->removed())
			{
				if (lpnode->m_parm.m_intervalms != nullptr)
				{
					const int lintervalms = lpnode->m_parm.m_intervalms(m_time_wheel->m_impl_time_wheel()->m_current_ms);
					--lpnode->m_parm.m_count;
					if (lintervalms > 0 && lpnode->m_parm.m_count > 0)
					{
						wheel_node* lpnewnode = new wheel_node(*lpnode);
						m_time_wheel->m_impl_time_wheel()->push(lpnewnode);
						lpnode->m_parm.m_ms += lintervalms;
						lpnode->m_next = lpushnode;
						lpushnode = lpnode;
						continue;
					}
				}
				m_time_wheel->m_impl_time_wheel()->push(lpnode);
				continue;
			}
			m_time_wheel->m_impl_time_wheel()->remove(lpnode);
		}
		m_slots[lpos] = nullptr;
		return lpushnode;
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