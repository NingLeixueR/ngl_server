#include "time_consuming.h"
#include "nprotocol.h"

namespace ngl
{

	struct tconsuming
	{
		int32_t			m_id;
		std::string		m_name;
		int64_t			m_beg;
		int64_t			m_end;
		std::function<bool(int64_t, int64_t)>	m_fun;
		static std::atomic<int32_t>				m_indexid;

		tconsuming(const std::string& aname, const std::function<bool(int64_t, int64_t)>& afun) :
			m_id(++m_indexid),
			m_name(aname),
			m_beg(time_wheel::getms()),
			m_end(0),
			m_fun(afun)
		{
		}
	};

	class tconsuming_thread
	{
		tconsuming_thread(const tconsuming_thread&) = delete;
		tconsuming_thread& operator=(const tconsuming_thread&) = delete;

		using ptr_tconsuming = std::shared_ptr<tconsuming>;

		ngl::thread					m_thread;
		std::shared_mutex			m_mutex;
		std::list<ptr_tconsuming>	m_list;
		std::map<int32_t, int64_t>	m_remove;

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

		void printf_log(bool awarn, const char* aname, const tconsuming* aitem)const
		{
			(awarn ? log_info() : log_error())->print("time consuming [{}:{}] [{}] [{}] ",
				aitem->m_name, aitem->m_id, aname, aitem->m_end - aitem->m_beg
			);
		}

		void run()
		{
			std::list<ptr_tconsuming>  llist;
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
				std::list<ptr_tconsuming> llist2;
				for (ptr_tconsuming& item : llist)
				{
					auto itor = lremove.find(item->m_id);
					if (itor != lremove.end())
					{
						item->m_end = itor->second;
						printf_log(item->m_fun(item->m_beg, item->m_end) == false, "finish", item.get());
						continue;
					}
					item->m_end = end;
					if (item->m_fun(item->m_beg, item->m_end) == false)
					{
						printf_log(false, "overtime", item.get());
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

	std::atomic<int32_t> tconsuming::m_indexid = 0;	

	time_consuming::time_consuming(const std::string& aname, const std::function<bool(int64_t, int64_t)>& afun)
	{
		m_id = tconsuming_thread::getInstance().add(aname, afun);
	}

	time_consuming::~time_consuming()
	{
		tconsuming_thread::getInstance().remove(m_id);
	}
}