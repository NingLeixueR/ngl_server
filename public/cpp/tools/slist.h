#pragma once

#include <functional>

#include "threadtools.h"
#include "time_wheel.h"
#include "localtime.h"
#include "type.h"
#include "nlog.h"

namespace ngl
{
	template <typename T>
	struct node
	{
		T m_data;
		node<T>* m_next = nullptr;
	};

	template <typename T>
	class lsnode
	{
		node<T>* m_head;
		node<T>* m_tail;
		int m_size;
	public:
		lsnode() :
			m_head(nullptr),
			m_tail(nullptr),
			m_size(0)
		{}

		lsnode(int asize) :
			m_head(nullptr),
			m_tail(nullptr),
			m_size(0)
		{
			expand(asize);
		}

		~lsnode()
		{
		}

		bool empty()const
		{
			return m_size <= 0;
		}

		void push_front(lsnode<T>& als)
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

		inline int expand(int asize)
		{// 只有freelist才会扩展
			for (int i = 0; i < asize; ++i)
			{
				push_back((node<T>*)std::malloc(sizeof(node<T>)));
			}
			return asize;
		}

		inline int size()const
		{ 
			return m_size; 
		}

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

		inline node<T>* front()
		{
			if (empty())
			{
				return nullptr;
			}
			return m_head;
		}

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

		inline void swap(lsnode<T>& adata)
		{
			std::swap(m_head, adata.m_head);
			std::swap(m_tail, adata.m_tail);
			std::swap(m_size, adata.m_size);
		}
	};

	template <typename T>
	class slist
	{
		int m_initcapacity;
		int m_expand;
		lsnode<T> m_list;
		lsnode<T> m_free;
		std::function<void(node<T>*)> m_listfree;
		std::function<void(node<T>*)> m_freefree;

		int m_maxsize;
	public:
		slist(int asize = 10, int amaxsize = 1000, int aexpand = 10) :
			m_expand(aexpand),
			m_initcapacity(asize),
			m_free(asize),
			m_maxsize(amaxsize),
			m_listfree([this](node<T>* adata)
				{
					std::destroy_at(adata);
					m_free.push_back(adata);
				}),
			m_freefree([this](node<T>* adata)
				{
					std::destroy_at(adata);
					std::free(adata);
				})
		{
		}

		~slist()
		{
			m_list.clear(m_freefree);
			m_free.clear(m_freefree);
		}

		inline void clear()
		{
			m_list.clear(m_listfree);
		}

		inline bool empty()
		{
			return m_list.size() == 0;
		}

		inline int size()
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

		bool operator_evaluation(node<T>* lpnode, const T& adata)
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

		inline void push_front(slist<T>& alist)
		{
			m_list.push_front(alist.m_list);
		}

		inline void swap(slist<T>& ar)
		{
			m_list.swap(ar.m_list);
			m_free.swap(ar.m_free);
			std::swap(m_initcapacity, ar.m_initcapacity);
			std::swap(m_expand, ar.m_expand);
			std::swap(m_maxsize, ar.m_maxsize);
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
			assert(ptr != nullptr);
			return ptr->m_data;
		}
	};
}
