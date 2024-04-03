#pragma once

#include "time_wheel.h"
#include "nlog.h"

namespace ngl
{
	struct tconsuming
	{
		int32_t m_id;
		std::string m_name;
		int64_t m_beg;
		int64_t m_end;
		std::function<bool(int64_t, int64_t)> m_fun;
		static int32_t m_indexid;

		tconsuming(const std::string& aname, const std::function<bool(int64_t, int64_t)>& afun) :
			m_id(++m_indexid),
			m_name(aname),
			m_beg(time_wheel::getms()),
			m_end(0),
			m_fun(afun)
		{
			LogLocalWarn("time consuming [%:%] start", aname, m_id);
		}

	};

	class tconsuming_thread
	{
		ngl::thread			m_thread;
		std::shared_mutex	m_mutex;
		std::list<tconsuming*> m_list;
		std::map<int32_t,int64_t> m_remove;

		tconsuming_thread() :
			m_thread(&tconsuming_thread::run, this)
		{}
	public:
		static tconsuming_thread& getInstance()
		{
			static tconsuming_thread ltemp;
			return ltemp;
		}

		int32_t add(const std::string& aname, const std::function<bool(int64_t, int64_t)>& afun)
		{
			monopoly_shared_lock(m_mutex);
			m_list.emplace_back(new tconsuming(aname, afun));
			return (*m_list.rbegin())->m_id;
		}

		void remove(int32_t aid)
		{
			monopoly_shared_lock(m_mutex);
			m_remove.insert(std::make_pair(aid, time_wheel::getms()));
		}

		void printf_log(bool awarn, const char* aname, tconsuming* aitem)
		{
			if (awarn)
			{
				LogLocalWarn("time consuming [%] finish [%]-[%]=[%] ", aname, aitem->m_end, aitem->m_beg, aitem->m_end - aitem->m_beg);
			}
			else
			{
				LogLocalError("time consuming [%] finish [%]-[%]=[%] ", aname, aitem->m_end, aitem->m_beg, aitem->m_end - aitem->m_beg);
			}
		}

		void run()
		{
			std::list<tconsuming*> llist;
			std::map<int32_t, int64_t> lremove;
			while (true)
			{
				ngl::sleep::seconds(1);
				{
					monopoly_shared_lock(m_mutex);
					llist.swap(m_list);
					lremove = m_remove;
					m_remove.clear();
				}
				int64_t end = time_wheel::getms();
				// ÒÆ³ý
				std::list<tconsuming*> llist2;
				for (tconsuming* item : llist)
				{
					auto itor = lremove.find(item->m_id);
					if (itor != lremove.end())
					{
						if (item->m_fun(item->m_beg, end) == true)
						{
							printf_log(true, "finish", item);
						}
						else
						{
							printf_log(false, "finish", item);
						}
						delete item;
						continue;
					}
					if (item->m_fun(item->m_beg, end) == false)
					{
						printf_log(false, "overtime", item);
					}
					llist2.push_back(item);
				}
				llist.clear();
				{
					monopoly_shared_lock(m_mutex);
					if (llist2.empty() == false)
					{
						m_list.insert(m_list.begin(), llist2.begin(), llist2.end());
					}
				}
				
			}
		}
	};

	class time_consuming
	{
		int32_t m_id;
	public:
		time_consuming(const std::string& aname, const std::function<bool(int64_t, int64_t)>& afun)
		{
			m_id = tconsuming_thread::getInstance().add(aname, afun);
		}

		~time_consuming()
		{
			tconsuming_thread::getInstance().remove(m_id);
		}
	};
}