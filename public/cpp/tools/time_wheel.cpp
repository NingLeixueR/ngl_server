#include "threadtools.h"
#include "time_wheel.h"

#include <iostream>

namespace ngl
{
	class wheel
	{
		friend class time_wheel;
		wheel(const wheel&) = delete;
		wheel& operator=(const wheel&) = delete;

		std::vector<wheel_node*>	m_slots;
		wheel*						m_nextround;
		wheel*						m_lastround;
		time_wheel*					m_time_wheel;

		int64_t			m_slot_ms;			 // ÿ����λ�ĺ�����
		int32_t			m_slot_count;		 // ��λ������
		int32_t			m_slot_less;		 // ȡ����
		int64_t			m_slot_sum_ms;		 // m_slot_ms * m_slot_count
		int32_t			m_current_pos;		 // ��ǰָ��
	public:
		wheel();
		inline void set(
			int64_t aslotms, int32_t aslotbit, wheel* anextround, 
			wheel* alastround, time_wheel* atime_wheel
		);

		inline int slot_count();

		inline int slot_less();

		inline int64_t all_slot_ms();

		inline bool push_slots(wheel_node* anode);

		inline bool push(wheel_node* anode);

		inline wheel_node* shift_current_pos(int apos, wheel* awheel);

		// ������Ҫ����Ľڵ�
		inline wheel_node* shift_current_pos(wheel* awheel);
	};


	struct time_wheel::impl_time_wheel
	{
		time_wheel_config	m_config;
		int64_t				m_server_start_ms;					// �����������ĺ���
		int64_t				m_current_ms;						// ��ǰ������
		std::vector<wheel*> m_wheel;
		std::thread*		m_thread;							// ʱ�����߳� 
		std::shared_mutex	m_mutex;
		bool				m_isthreadcallback;					// �Ƿ�ʹ�� [ʹ���߳��Զ�����]
		// ###### ʹ���߳��Զ����� start 
		std::thread*		m_threadcallback;					// ʱ���ֹ����߳�����ִ�лص�
		std::shared_mutex	m_mutexcallback;
		ngl::sem			m_sem;
		// ###### ʹ���߳��Զ����� finish
		wheel_node*			m_worldnodehead;
		wheel_node*			m_worldnodetail;
		int64_t				m_timerid;							// ��ʱ������id
		std::map<int64_t, bool> m_timer;						// ���ڿ���ɾ����ʱ��
		time_wheel*			m_twheel;
		bool				m_stop;

		impl_time_wheel(time_wheel* atwheel, const time_wheel_config& aconfig, bool aisthreadcallback):
			m_twheel(atwheel),
			m_isthreadcallback(aisthreadcallback),
			m_thread(nullptr),
			m_timerid(1),
			m_worldnodehead(nullptr),
			m_worldnodetail(nullptr),
			m_config(aconfig),
			m_stop(false)
		{
			m_server_start_ms = getms();
			m_current_ms = m_server_start_ms;
			int64_t lms = m_config.m_time_wheel_precision;
			m_wheel.resize(m_config.m_time_wheel_count);
				
			for (int32_t i = 0; i < m_config.m_time_wheel_count; ++i)
			{
				m_wheel[i] = new wheel();
			}

			for (int32_t i = 0; i < m_config.m_time_wheel_count; ++i)
			{
				m_wheel[i]->set(
					lms, m_config.m_time_wheel_bit, 
					(i + 1 < m_config.m_time_wheel_count) ? m_wheel[i + 1] : nullptr,
					(i == 0) ? nullptr : m_wheel[i - 1], atwheel
				);
				lms = m_wheel[i]->all_slot_ms();
			}

			m_thread = new std::thread(&impl_time_wheel::run, this);
			if (m_isthreadcallback)
			{
				m_threadcallback = new std::thread(&impl_time_wheel::runcallback, this);
			}
			else
			{
				m_threadcallback = nullptr;
			}
		}

		~impl_time_wheel()
		{
			stop();
			for (auto item : m_wheel)
				delete item;
			m_wheel.clear();
		}

		void remove(wheel_node* anode)
		{
			wheel_node* lpnextnode = nullptr;
			for (wheel_node* lpnode = anode; lpnode != nullptr; lpnode = lpnextnode)
			{
				std::unique_ptr<wheel_node> ltemp(lpnode);
				lpnextnode = lpnode->m_next;
				auto itor = m_timer.find(lpnode->m_timerid);
				if (itor == m_timer.end())
					continue;
				m_timer.erase(itor);
			}
		}

		int64_t all_slot_ms()
		{
			return  m_wheel[m_config.m_time_wheel_count - 1]->all_slot_ms();
		}

		int64_t duration_ms()
		{
			return   m_current_ms - m_server_start_ms;
		}

		bool addtimer(wheel_node* apnode)
		{
			wheel_node* lpnextnode = nullptr;
			wheel_node* removenode = nullptr;
			int64_t lduration = duration_ms();
			int64_t lallslot = all_slot_ms();
			bool lbool = false;
			for (wheel_node* lpnode = apnode; lpnode != nullptr; lpnode = lpnextnode)
			{
				lbool = false;
				lpnextnode = lpnode->m_next;
				lpnode->m_next = nullptr;

				if (lpnode->m_parm.m_ms > lallslot)
				{//��ʱʱ�䳬����ʱ�����ܴﵽ�����ʱ��
					continue;
				}
				
				if (lpnode->m_parm.m_ms < lduration)
				{// �����ʱʱ������ǰ ��ôֱ��ִ�ж�ʱ����
					int lintervalms = lpnode->m_parm.m_intervalms(m_current_ms);
					bool lcopy = lintervalms > 0 && lpnode->m_parm.m_count - 1 > 0;
					if (lcopy)//����
					{
						wheel_node* lpnewnode = new wheel_node(*lpnode);
						push(lpnewnode);
						--lpnode->m_parm.m_count;
						lpnode->m_parm.m_ms += lintervalms;
						addtimer(lpnode);
					}
					else
						push(apnode);
					continue;
				}
				auto& ltimer = m_timer;
				auto& lwheel = m_wheel;
				for (wheel* item : lwheel)
				{
					if (item->push(lpnode))
					{
						lbool = true;
						m_timer.insert(std::make_pair(m_timerid, &lpnode->m_remove));
						break;
					}
				}
				if (lbool == false)
				{
					lpnode->m_next = removenode;
					removenode = lpnode;
				}
			}
			if (removenode != nullptr)
				remove(removenode);
			return true;
		}

		void removetimer(int64_t atimerid)
		{
			monopoly_shared_lock(m_mutex);
			auto itor = m_timer.find(atimerid);
			if (itor == m_timer.end())
				return;
			itor->second = true;
		}

		void push(wheel_node* apnode)
		{
			wheel_node* ltailnode = nullptr;
			if (m_isthreadcallback)
			{
				monopoly_shared_lock(m_mutexcallback);
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
			}
			else
			{
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
		}
		
		void stop()
		{
			m_stop = true;
			m_sem.post();
		}

		void run()
		{
			int32_t ltemp = 0;
			int32_t ltempsleep = 0;
			while (m_stop == false)
			{
				ltemp = getms() - m_current_ms;
				ltempsleep = m_config.m_time_wheel_precision - ltemp;
				if (ltempsleep > 0)
					std::this_thread::sleep_for(std::chrono::milliseconds(ltempsleep));
				monopoly_shared_lock(m_mutex);
				wheel_node* lpbnode = m_wheel[0]->shift_current_pos(nullptr);
				if (lpbnode != nullptr)
					addtimer(lpbnode);
				m_current_ms += m_config.m_time_wheel_precision;
			}
		}

		void runcallback()
		{
			wheel_node* lpnode = nullptr;
			while (m_stop == false)
			{
				m_sem.wait();
				{
					monopoly_shared_lock(m_mutexcallback);
					std::swap(lpnode, m_worldnodehead);
					m_worldnodetail = nullptr;
				}
				wheel_node* lnextnode = nullptr;
				for (wheel_node* pnode = lpnode; pnode != nullptr; pnode = lnextnode)
				{
					lnextnode = pnode->m_next;
					if (pnode->m_remove != true)
						pnode->m_parm.m_fun(pnode);
					delete pnode;
				}
				lpnode = nullptr;
			}
		}

		std::shared_ptr<wheel_node> pop_node()
		{
			wheel_node* ret = nullptr;
			if (m_worldnodehead == m_worldnodetail)
			{
				if (m_worldnodehead == nullptr)
					return nullptr;
				ret = m_worldnodehead;
				m_worldnodetail = nullptr;
				m_worldnodehead = nullptr;
				return std::shared_ptr<wheel_node>(ret);
			}
			else
			{
				ret = m_worldnodehead;
				m_worldnodehead = m_worldnodehead->m_next;
				return std::shared_ptr<wheel_node>(ret);
			}
		}

		int64_t addtimer(const wheel_parm& apram)
		{
			bool lbool = true;
			std::shared_ptr<wheel_node> lpnode(
				new wheel_node(m_twheel, ++m_timerid, apram),
				[&lbool](wheel_node* ap) 
				{
					if (lbool)
						delete ap; 
				});
			lpnode->m_parm.m_timerstart = getms();
			lpnode->m_parm.m_ms += getms() - m_server_start_ms;
			if (lpnode->m_parm.m_ms < 0)
				return -1;
			if (lpnode->m_parm.m_count == -1)
				lpnode->m_parm.m_count = 0x7fffffff;
			if (addtimer(lpnode.get()) == false)
				return -1;
			lbool = false;
			return m_timerid;
		}

		bool& get_remove(int64_t atimerid)
		{
			auto itor = m_timer.find(atimerid);
			if (itor == m_timer.end())
			{
				bool& ret = m_timer[atimerid];
				ret = false;
				return ret;
			}
			return itor->second;
		}
	};

	wheel_node::wheel_node(time_wheel* atw, int64_t atimerid, const wheel_parm& aparm) :
		m_tw(atw)
		, m_remove(atw->get_remove(atimerid))
		, m_timerid(atimerid)
		, m_next(nullptr)
		, m_parm(aparm)
	{}

	wheel_node::wheel_node(const wheel_node& aparm) :
		m_tw(aparm.m_tw)
		, m_remove(aparm.m_remove)
		, m_timerid(aparm.m_timerid)
		, m_next(nullptr)
		, m_parm(aparm.m_parm)
	{}

	wheel::wheel() :
		m_nextround(nullptr),
		m_lastround(nullptr),
		m_time_wheel(nullptr),
		m_slot_ms(0),
		m_slot_count(0),
		m_slot_less(0),
		m_slot_sum_ms(0),
		m_current_pos(0)
	{}

	void wheel::set(int64_t aslotms, int32_t aslotbit, wheel* anextround, wheel* alastround, time_wheel* atime_wheel)
	{
		m_nextround = anextround;
		m_slot_ms = aslotms;
		m_time_wheel = atime_wheel;
		m_slot_count = (1 << aslotbit);
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
		int lcallcount = anode->m_parm.m_ms / m_slot_ms;
		lcallcount &= m_slot_less;//lcallcount %= m_slot_count;
		int lcount = m_current_pos & m_slot_less;
		if (lcallcount <= lcount && m_current_pos != -1)//if (lcallcount <= m_current_pos% m_slot_count)
		{
			if (m_lastround != nullptr)
			{
				m_lastround->push_slots(anode);
				return true;
			}
			else if (lcallcount != lcount)
			{
				//log_error()->print("wheel.push_slots({},{})", lcallcount, lcount);
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
			return false;
		return push_slots(anode);
	}

	wheel_node* wheel::shift_current_pos(int apos, wheel* awheel)
	{
		for (wheel_node* lpnode = m_slots[apos], *ltempnode = nullptr; lpnode != nullptr; lpnode = ltempnode)
		{
			ltempnode = lpnode->m_next;
			lpnode->m_next = nullptr;
			if (awheel->push_slots(lpnode))
				continue;
			m_time_wheel->m_impl_time_wheel()->remove(lpnode);
		}
		m_slots[apos] = nullptr;
		return nullptr;
	}

	wheel_node* wheel::shift_current_pos(wheel* awheel)
	{
		++m_current_pos;
		int lpos = m_current_pos & m_slot_less;
		if (lpos == 0 && m_current_pos != 0)//if (m_current_pos % m_slot_count == 0 && m_current_pos != 0)
		{
			if (m_nextround == nullptr)
				return nullptr;
			m_nextround->shift_current_pos(this);
			if (awheel != nullptr)
				return shift_current_pos(lpos, awheel);
		}
		else
		{
			if (awheel != nullptr)
				return shift_current_pos(lpos, awheel);
		}
		wheel_node* lpushnode = nullptr;
		for (wheel_node* lpnode = m_slots[lpos], *ltempnode = nullptr; lpnode != nullptr; lpnode = ltempnode)
		{
			ltempnode = lpnode->m_next;
			lpnode->m_next = nullptr;
			if (lpnode->m_remove != true)
			{
				//lpnode->m_fun(m_current_ms);
				if (lpnode->m_parm.m_intervalms != nullptr)
				{
					int lintervalms = lpnode->m_parm.m_intervalms(
						m_time_wheel->m_impl_time_wheel()->m_current_ms);
					if (lintervalms > 0 && --lpnode->m_parm.m_count > 0)//����
					{
						wheel_node* lpnewnode = new wheel_node(*lpnode);
						m_time_wheel->m_impl_time_wheel()->push(lpnewnode);
						lpnode->m_parm.m_ms += lintervalms;
						lpnode->m_next = lpushnode;
						lpushnode = lpnode;
					}
					else
					{
						m_time_wheel->m_impl_time_wheel()->push(lpnode);
					}
					continue;
				}
			}
			m_time_wheel->m_impl_time_wheel()->remove(lpnode);
		}
		m_slots[lpos] = nullptr;
		return lpushnode;
	}

	int64_t time_wheel::getms()
	{
		return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
	}

	time_wheel::time_wheel(const time_wheel_config& aconfig, bool aisthreadcallback /*= true*/)
	{
		m_impl_time_wheel.make_unique(this, aconfig, aisthreadcallback);
	}

	time_wheel::~time_wheel()
	{
	}

	int	time_wheel::count() 
	{ 
		monopoly_shared_lock(m_impl_time_wheel()->m_mutex);
		return  m_impl_time_wheel()->m_timer.size();
	}

	bool time_wheel::empty() 
	{ 
		monopoly_shared_lock(m_impl_time_wheel()->m_mutex);
		return  m_impl_time_wheel()->m_timer.empty();
	}

	int64_t time_wheel::server_start_ms() 
	{ 
		monopoly_shared_lock(m_impl_time_wheel()->m_mutex);
		return  m_impl_time_wheel()->m_server_start_ms;
	}

	int64_t time_wheel::server_current_ms() 
	{ 
		monopoly_shared_lock(m_impl_time_wheel()->m_mutex);
		return  m_impl_time_wheel()->m_current_ms; 
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

	bool& time_wheel::get_remove(int64_t atimerid)
	{
		return m_impl_time_wheel()->get_remove(atimerid);
	}

	time_wheel twheel::m_wheel;
}// namespace ngl