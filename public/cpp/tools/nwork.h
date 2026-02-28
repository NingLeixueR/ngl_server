/*
* Copyright (c) [2020-2025] NingLeixueR
* 
* 项目名称：ngl_server
* 项目地址：https://github.com/NingLeixueR/ngl_server
* 
* 本文件是 ngl_server 项目的一部分，遵循 MIT 开源协议发布。
* 您可以按照协议规定自由使用、修改和分发本项目，包括商业用途，
* 但需保留原始版权和许可声明。
* 
* 许可详情参见项目根目录下的 LICENSE 文件：
* https://github.com/NingLeixueR/ngl_server/blob/main/LICENSE
*/
#pragma once

#include "tools/threadtools.h"

namespace ngl
{
	template <typename T>
	class nwork
	{
		nwork() = delete;
		nwork(const nwork&) = delete;
		nwork& operator=(const nwork&) = delete;

		std::list<std::shared_ptr<T>>	m_list;
		std::list<std::shared_ptr<T>>	m_copylist;
		std::jthread					m_thread;
		std::function<void(T&)>			m_fun = nullptr;
		std::shared_mutex				m_mutex;
		ngl::sem						m_sem;
	public:
		nwork(const std::function<void(T&)>& afun) :
			m_fun(afun),
			m_thread(std::bind_front(&nwork::run, this))
		{
		}

		void run()
		{
			while (true)
			{
				m_sem.wait();
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
			ngl_post;
		}
	};
}//namespace ngl