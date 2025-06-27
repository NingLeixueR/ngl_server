#include "threadtools.h"
#include "slist.h"

namespace ngl
{
	template <typename T>
	class worklist
	{
		worklist() = delete;
		worklist(const worklist&) = delete;
		worklist& operator=(const worklist&) = delete;

		ngl_lockinit;

		slist_production<T>			m_list;
		slist_consumption<T>		m_copylist;
		std::function<void(T&)>		m_fun;
		std::thread					m_thread;
	public:
		worklist(const std::function<void(T&)>& afun) :
			m_fun(afun),
			m_thread([this] { run(); })
		{
		}

		void run()
		{
#ifndef OPEN_SEM
			auto lfun = [this]() {return !m_list.empty(); };
#endif//OPEN_SEM
			while (true)
			{
#ifdef OPEN_SEM
				m_sem.wait();
				{
					monopoly_shared_lock(m_mutex);
					if (m_list.empty())
					{
						continue;
					}
					m_list.swap(m_copylist);
				}
#else
				{
					cv_lock(m_cv, m_mutex, lfun)
					m_list.swap(m_copylist);
				}
#endif//OPEN_SEM 
				m_copylist.foreach([this](T& adata)
					{
						m_fun(adata);
					});
				m_copylist.clear();
				m_list.push_front(m_copylist);
			}
		}

		inline void push_back(T&& anode)
		{
			{
				monopoly_shared_lock(m_mutex);
				m_list.push_back(anode);
			}
			ngl_post;
		}

		inline void push_back(T& anode)
		{
			{
				monopoly_shared_lock(m_mutex);
				m_list.push_back(anode);
			}
			ngl_post;
		}
	};
}//namespace ngl