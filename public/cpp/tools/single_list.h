#pragma once

#include <functional>
#include "localtime.h"
#include "type.h"
#include "threadtools.h"
#include "time_wheel.h"
#include "nlog.h"

namespace ngl
{
	template <typename T>
	struct list_node
	{
		T m_data;
		list_node<T>* m_next;
	};

	template <typename T>
	class list_base
	{
		list_node<T>* m_head;
		list_node<T>* m_tail;
		int m_size;
	public:
		list_base() :
			m_head(nullptr),
			m_tail(nullptr),
			m_size(0)
		{}

		list_base(int asize) :
			m_head(nullptr),
			m_tail(nullptr),
			m_size(0)
		{
			resize(asize);
		}

		~list_base()
		{
			clear();
		}

		list_node<T>* begin()
		{
			return m_head;
		}

		list_node<T>* end()
		{
			return nullptr;
		}

		void insert(list_node<T>* ainsert, list_node<T>* abeg, list_node<T>* aend)
		{
			if (ainsert == nullptr || ainsert == m_tail)
			{
				for (auto itor = abeg; itor != aend ; ++itor)
				{

				}
			}
		}

		inline void resize(int asize)
		{
			if (m_size < asize)
			{
				for (int i = asize - m_size; i < asize; ++i)
					push_back(new list_node<T>());
			}
			else if (m_size > asize)
			{
				release(m_size - asize);
			}			
		}

		inline int expand(int asize)
		{
			for (int i = 0; i < asize; ++i)
			{
				push_back(new list_node<T>());
			}
			return asize;
		}

		inline int size() 
		{ 
			return m_size; 
		}

		inline int empty() 
		{ 
			return m_size == 0; 
		}


		inline void clear()
		{
			for (list_node<T>* beg = m_head, lptemp = nullptr; m_size > 0; beg = lptemp)
			{
				lptemp = beg->m_next;
				--m_size;
				delete beg;
			}
		}

		inline int release(int acount)
		{
			int lcount = m_size - acount;
			list_node<T>* beg = m_head;
			for (list_node<T>* lptemp = nullptr; m_size > lcount && m_size > 0; )
			{
				lptemp = beg->m_next;
				--m_size;
				delete beg;
				beg = lptemp;
			}
			m_head = beg;
			if (m_size <= 1)
				m_tail = m_head;
			return acount;
		}

		inline void foreach(list_base<T>& alist, const std::function<void(T&)>& afun, int acout = 0)
		{
			list_node<T>* node = pop();
			for (int i = 0; (acout <= 0 || i < acout) && node != nullptr; ++i, node = pop())
			{
				afun(node->m_data);
				node->m_data = T();
				alist.push_back(node);
			}
		}

		inline void push_back(list_node<T>* adata)
		{
			if (adata == nullptr) return;
			adata->m_next = nullptr;
			if (m_size == 0)
			{
				m_head = adata;
				m_tail = adata;
			}
			else
			{
				m_tail->m_next = adata;
				m_tail = adata;
			}
			++m_size;
		}

		inline void push_front(list_node<T>* adata)
		{
			if (adata == nullptr) return;
			adata->m_next = nullptr;
			if (m_size == 0)
			{
				m_head = adata;
				m_tail = adata;
			}
			else
			{
				list_node<T>* ltemp = m_head;
				m_head = adata;
				m_head->m_next = ltemp;
			}
			++m_size;
		}

		inline list_node<T>* pop()
		{
			if (m_size <= 0)
				return nullptr;
			list_node<T>* ret = m_head;
			if (m_head == m_tail)
			{
				m_head = nullptr;
				m_tail = nullptr;
				m_size = 0;
				return ret;
			}
			m_head = m_head->m_next;
			--m_size;
			return ret;
		}

		inline void swap(list_base<T>& adata)
		{
			std::swap(m_head, adata.m_head);
			std::swap(m_tail, adata.m_tail);
			std::swap(m_size, adata.m_size);
		}

		inline void insert_beg(list_base<T>& adata)
		{
			adata.m_tail->m_next = m_head;
			m_head = adata.m_head;
			m_size += adata.m_size;
		}

		inline void insert_end(list_base<T>& adata)
		{
			m_tail->m_next = adata.m_head;
			m_tail = adata.m_tail;
			m_size += adata.m_size;
		}

		// #### adata 会失效 防止其被误操作
		inline void insert_range(list_base<T>& adata, bool ahead)
		{
			if (adata.m_size == 0)	return;
			if (m_size == 0)
				swap(adata);
			else
			{
				if (ahead)
					insert_beg(adata);
				else
					insert_end(adata);
			}
			adata.reset();
		}
	};

	template <typename T>
	class slist
	{
		int m_initcapacity;
		int m_expand;
		list_base<T> m_list;
		list_base<T> m_free;

		int m_maxsize;
		int m_count;
	public:
		slist(int asize = 10, int amaxsize = 1000, int aexpand = 10) :
			m_expand(aexpand),
			m_initcapacity(asize),
			m_free(asize),
			m_maxsize(amaxsize),
			m_count(asize)
		{
			m_free.reset();
		}

		inline void clear()
		{
			m_list.clear();
			m_free.resize(asize);
		}

		inline bool empty()
		{
			return m_list.size() == 0;
		}

		inline int size()
		{
			return m_list.size();
		}

		inline list_node<T>* get_freenode()
		{
			list_node<T>* lpnode = m_free.pop();
			if (lpnode == nullptr)
			{
				if (m_count >= m_maxsize)
					return false;
				m_count += m_free.expand(m_expand);
				lpnode = m_free.pop();
			}
			return lpnode;
		}

		inline bool push_back(const T& adata)
		{
			list_node<T>* lpnode = get_freenode();
			lpnode->m_data = adata;
			m_list.push_back(lpnode);
			return true;
		}

		inline bool push_back(const T*& adata)
		{
			list_node<T>* lpnode = get_freenode();
			lpnode->m_data = *adata;
			adata = nullptr;
			m_list.push_back(lpnode);
			return true;
		}

		inline bool push_back(list_node<T>* adata)
		{
			m_list.push_back(adata);
			return true;
		}

		inline bool push_front(const T& adata)
		{
			list_node<T>* lpnode = get_freenode();
			lpnode->m_data = adata;
			m_list.push_front(lpnode);
			return true;
		}

		inline bool push_front(const T*& adata)
		{
			list_node<T>* lpnode = get_freenode();
			lpnode->m_data = *adata;
			adata = nullptr;
			m_list.push_front(lpnode);
			return true;
		}

		inline bool push_front(list_node<T>* adata)
		{
			m_list.push_front(adata);
			return true;
		}

		inline void swap(const slist<T>& ar)
		{
			m_list.swap(ar);
		}
	};


}
