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

#include <functional>
#include <iostream>

#include "tools.h"

namespace ngl
{
	template <typename T>
	struct node
	{
		T m_data;
		node<T>* m_next = nullptr;

		inline node<T>* next()
		{
			return m_next;
		}
	};

	template <typename T>
	class lsnode
	{
		node<T>* m_head = nullptr;
		node<T>* m_tail = nullptr;
		int m_size = 0;
	public:
		inline lsnode() :
			m_head(nullptr),
			m_tail(nullptr),
			m_size(0)
		{}

		inline lsnode(int asize) :
			m_head(nullptr),
			m_tail(nullptr),
			m_size(0)
		{
			expand(asize);
		}

		~lsnode()
		{
		}

		// # 队列是否为空
		inline bool empty()const
		{
			return m_size <= 0;
		}

		// # 队头
		inline node<T>* begin()
		{
			return m_head;
		}

		// # 队尾
		inline node<T>* end()
		{
			return nullptr;
		}

		// # 在队头加入一组结点
		inline void push_front(lsnode<T>& als)
		{
			if (als.empty())
			{
				return;
			}
			if (empty())
			{
				m_head = als.m_head;
				m_tail = als.m_tail;
				m_size = als.size();
			}
			else
			{
				als.m_tail->m_next = m_head;
				m_head = als.m_head;
				m_size += als.m_size;
			}
			als.clear(nullptr);
		}

		// # 在队头加入结点
		inline void push_front(node<T>* adata)
		{
			if (adata == nullptr)
			{
				return;
			}
			adata->m_next = nullptr;
			if (empty())
			{
				m_head = adata;
				m_tail = adata;
				m_size = 1;
			}
			else
			{
				adata->m_next = m_head;
				m_head = adata;
				++m_size;
			}
		}

		// # 在队尾加入结点
		inline void push_back(node<T>* adata)
		{
			if (adata == nullptr)
			{
				return;
			}
			adata->m_next = nullptr;
			if (empty())
			{
				m_head = adata;
				m_tail = adata;
				m_size = 1;
			}
			else
			{
				m_tail->m_next = adata;
				m_tail = adata;
				++m_size;
			}
		}

		// # 扩展队列空间
		inline int expand(int asize)
		{// 只有freelist才会扩展
			for (int i = 0; i < asize; ++i)
			{
				push_back((node<T>*)std::malloc(sizeof(node<T>)));
			}
			return asize;
		}

		// # 结点数量
		inline int size()const
		{ 
			return m_size; 
		}

		// # 清空列表
		inline void clear(const std::function<void(node<T>*)>& afun)
		{
			if (afun != nullptr)
			{
				node<T>* lptemp = nullptr;
				for (; m_size > 0 && m_head != nullptr;)
				{
					lptemp = m_head;
					m_head = m_head->m_next;
					afun(lptemp);
				}
			}			
			m_head = nullptr;
			m_tail = nullptr;
			m_size = 0;
		}

		// # 对头
		inline node<T>* front()
		{
			if (empty())
			{
				return nullptr;
			}
			return m_head;
		}

		// # 弹出一个队头结点
		inline void pop(const std::function<void(node<T>*)>& afun)
		{
			if (empty())
			{
				return;
			}
			node<T>* lptemp = m_head;
			m_head = m_head->m_next;
			lptemp->m_next = nullptr;
			if (afun != nullptr)
			{
				afun(lptemp);
			}
			--m_size;
			if (m_head == nullptr)
			{
				m_size = 0;
				m_tail = nullptr;
			}
		}

		// # 交换两个队列
		inline void swap(lsnode<T>& adata)noexcept
		{
			std::swap(m_head, adata.m_head);
			std::swap(m_tail, adata.m_tail);
			std::swap(m_size, adata.m_size);
		}
	};

	// 消费队列
	template <typename T>
	class slist_consumption
	{
		lsnode<T> m_list;
		lsnode<T> m_free;
		std::function<void(node<T>*)> m_listfree = nullptr;
	public:
		inline slist_consumption()
		{
			m_listfree = [this](node<T>* adata)
				{
					std::destroy_at(adata);
					m_free.push_back(adata);
				};
		}

		inline lsnode<T>& get_list()
		{
			return m_list;
		}

		inline lsnode<T>& get_free()
		{
			return m_free;
		}

		inline bool empty()const
		{
			return m_list.size() == 0;
		}

		inline int size()const
		{
			return m_list.size();
		}

		inline void pop_front()
		{
			m_list.pop(m_listfree);
		}

		inline T& front()
		{
			auto ptr = m_list.front();
			if (ptr == nullptr)
			{
				tools::no_core_dump();
			}
			return ptr->m_data;
		}

		inline void clear()
		{
			m_list.clear(m_listfree);
		}

		inline void foreach(const std::function<void(T&)>& afun)
		{
			auto itor = m_list.begin();
			while (itor != m_list.end())
			{
				afun(itor->m_data);
				itor = itor->next();
			}
		}
	};

	// 生产队列
	template <typename T>
	class slist_production
	{
		int m_initcapacity = 0;
		int m_expand = 0;
		int m_maxsize = 0;
		lsnode<T> m_list;
		lsnode<T> m_free;
		std::function<void(node<T>*)> m_listfree = nullptr;
		std::function<void(node<T>*)> m_freefree = nullptr;
	public:
		enum
		{
			eslist_size = 10,
			eslist_maxsize = 1000,
			eslist_expand = 10,
		};
		inline slist_production(int asize = eslist_size, int amaxsize = eslist_maxsize, int aexpand = eslist_expand) :
			m_expand(aexpand),
			m_initcapacity(asize),
			m_free(asize),
			m_maxsize(amaxsize)
		{
			m_listfree = [this](node<T>* adata)
				{
					std::destroy_at(adata);
					m_free.push_back(adata);
				};
			m_freefree = [this](node<T>* adata)
				{
					std::destroy_at(adata);
					std::free(adata);
				};
		}

		~slist_production()
		{
			try
			{
				m_list.clear(m_freefree);
				m_free.clear(m_freefree);
			}
			catch(...)
			{
				std::cout << "throw ~slist_production()" << std::endl;
			}
		}

		inline void clear()
		{
			m_list.clear(m_listfree);
		}

		inline bool empty()const
		{
			return m_list.size() == 0;
		}

		inline int size()const
		{
			return m_list.size();
		}

		inline int capacity()
		{
			return m_list.size() + m_free.size();
		}

	private:
		inline node<T>* get_freenode()
		{
			node<T>* lpnode = m_free.front();
			if (lpnode == nullptr)
			{
				if (capacity() < m_maxsize)
				{
					m_free.expand(m_expand);
					lpnode = m_free.front();
					m_free.pop(nullptr);
				}
				else
				{
					lpnode = (node<T>*)std::malloc(sizeof(node<T>));
				}
			}
			else
			{
				m_free.pop(nullptr);
			}
			lpnode = new (lpnode)node<T>();
			return lpnode;
		}

		inline bool operator_evaluation(node<T>* lpnode, const T& adata)
		{
			try
			{
				lpnode->m_data = adata;
			}
			catch (...)
			{
				m_free.push_back(lpnode);
				return false;
			}
			return true;
		}

	public:
		inline bool push_back(const T& adata)
		{
			node<T>* lpnode = get_freenode();
			if (operator_evaluation(lpnode, adata))
			{
				m_list.push_back(lpnode);
				return true;
			}
			return false;
		}

		inline bool push_back(const T* adata)
		{
			node<T>* lpnode = get_freenode();
			if (operator_evaluation(lpnode, *adata))
			{
				m_list.push_back(lpnode);
				return true;
			}
			return false;
		}

		inline bool push_back(node<T>* adata)
		{
			m_list.push_back(adata);
			return true;
		}

		inline bool push_front(const T& adata)
		{
			node<T>* lpnode = get_freenode();
			if (operator_evaluation(lpnode, adata))
			{
				m_list.push_front(lpnode);
				return true;
			}
			return false;
		}

		inline bool push_front(const T* adata)
		{
			node<T>* lpnode = get_freenode();
			if (operator_evaluation(lpnode, *adata))
			{
				m_list.push_front(lpnode);
				return true;
			}
			return false;
		}

		inline bool push_front(node<T>* adata)
		{
			m_list.push_front(adata);
			return true;
		}

		inline void push_front(slist_consumption<T>& alist)
		{
			m_list.push_front(alist.get_list());
			m_free.push_front(alist.get_free());
		}

		inline void swap(slist_consumption<T>& ar)noexcept
		{
			m_list.swap(ar.get_list());
		}

		inline void pop_front()
		{
			if (capacity() > m_maxsize)
			{
				m_list.pop(m_freefree);
			}
			else
			{
				m_list.pop(m_listfree);
			}
		}

		inline T& front()
		{
		 	auto ptr = m_list.front();
			if (ptr == nullptr)
			{
				tools::no_core_dump();
			}
			return ptr->m_data;
		}
	};

	extern void test_slist();
}//namespace ngl