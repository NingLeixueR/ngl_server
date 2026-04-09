/*
* Copyright (c) [2020-2025] NingLeixueR
* 
* Project name: ngl_server
* Project URL: https://github.com/NingLeixueR/ngl_server
* 
* This file is part of the ngl_server project and is distributed under the MIT License.
* You may use, modify, and distribute this project under the license, including commercial use,
* but you must retain the original copyright and license notice.
* 
* For license details, see the LICENSE file in the project root:
* https://github.com/NingLeixueR/ngl_server/blob/main/LICENSE
*/
// File overview: Declares interfaces for tools.

#pragma once

#include "tools/tools/tools_thread.h"

namespace ngl::tools
{
	template <typename T>
	class nwork
	{
		nwork() = delete;
		nwork(const nwork&) = delete;
		nwork& operator=(const nwork&) = delete;

		std::list<std::shared_ptr<T>>	m_list;
		std::list<std::shared_ptr<T>>	m_copylist;
		std::function<void(T&)>			m_fun = nullptr;
		std::shared_mutex				m_mutex;
		ngl::tools::sem						m_sem;
		std::jthread					m_thread;		// Start last so queue state exists before run().
	public:
		nwork(const std::function<void(T&)>& afun) :
			m_fun(afun),
			m_thread([this](std::stop_token astop)
				{
					run(astop);
				})
		{
		}

		~nwork()
		{
			m_thread.request_stop();
			m_sem.post();
			if (m_thread.joinable())
			{
				m_thread.join();
			}
		}

		void run(std::stop_token astop)
		{
			while (!astop.stop_requested())
			{
				m_sem.wait();
				if (astop.stop_requested())
				{
					break;
				}
				{
					lock_write(m_mutex);
					if (m_list.empty())
					{
						continue;
					}
					m_list.swap(m_copylist);
				}
				std::ranges::for_each(m_copylist, [&](std::shared_ptr<T>& adata)
					{
						m_fun(*adata);
					}
				);
				m_copylist.clear();
			}
		}

		template <typename ...ARGS>
		static std::shared_ptr<T> make_shared(ARGS&... args)
		{
			return std::make_shared<T>(args...);
		}

		inline void push_back(std::shared_ptr<T>& anode)
		{
			{
				lock_write(m_mutex);
				m_list.push_back(anode);
			}
			m_sem.post();
		}
	};
}//namespace ngl::tools
