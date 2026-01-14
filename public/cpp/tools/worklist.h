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
		std::thread					m_thread;
		std::function<void(T&)>		m_fun = nullptr;
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
					}
				);
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