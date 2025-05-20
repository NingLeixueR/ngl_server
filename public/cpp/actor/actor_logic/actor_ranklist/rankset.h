#pragma once

#include "ttab_ranklist.h"
#include "rank_item.h"

namespace ngl
{
	template <pbdb::eranklist RTYPE>
	class operator_value
	{
	public:
		bool operator()(rank_item* al, rank_item* ar)const
		{
			return al->compare(RTYPE, *ar);
		}
	};

	class rankset_base
	{
		int32_t m_count;
	public:
		rankset_base() :
			m_count(-1)
		{}

		virtual void erase(rank_item* aitem) = 0;
		virtual void insert(rank_item* aitem) = 0;
		virtual void foreach(const std::function<void(int32_t, const rank_item*)>&) = 0;
		virtual int32_t getpage(int32_t apage, const std::function<void(int32_t, const rank_item*)>& afun) = 0;
		virtual void set_count(int32_t acount)
		{
			m_count = acount;
		}
		virtual int32_t count()
		{
			return m_count;
		}
		virtual void check() = 0;
	};

	template <pbdb::eranklist ETYPE>
	class rankset :
		public rankset_base
	{
		using setitor = std::set<rank_item*, operator_value<ETYPE>>::iterator;
		std::set<rank_item*, operator_value<ETYPE>> m_rankdata;
		int64_t m_time = 0; // 更新时间
		std::vector<setitor> m_page;
		int64_t m_pagetime = 0; // m_page更新时间

		const tab_ranklist* tab()
		{
			auto ltab = ttab_ranklist::tab(ETYPE);
			assert(ltab != nullptr);
			return ltab;
		}

		void init()
		{
			m_page.clear();
			auto ltab = tab();
			int pagesize = ltab->m_showitem / ltab->m_everypagecount;
			m_page.resize(pagesize);

			auto itor = m_rankdata.begin();
			for (int i = 0; i < pagesize; ++i)
			{
				m_page[i] = itor;
				if (itor != m_rankdata.end())
				{
					std::advance(itor, ltab->m_everypagecount);
				}
			}
			m_pagetime = m_time;
		}
	public:
		virtual void erase(rank_item* aitem)
		{
			m_rankdata.erase(aitem);
			m_time = localtime::gettime();
		}

		virtual void insert(rank_item* aitem)
		{
			auto ltab = tab();
			m_rankdata.insert(aitem);
			while (m_rankdata.size() > ltab->m_maxitem)
			{
				auto itor = std::prev(m_rankdata.end());
				m_rankdata.erase(itor);
			}
			m_time = localtime::gettime();
		}

		virtual void foreach(const std::function<void(int32_t, const rank_item*)>& afun)
		{
			int32_t lindex = 0;
			for (const rank_item* item : m_rankdata)
			{
				afun(lindex++, item);
			}
		}

		virtual int32_t getpage(int32_t apage, const std::function<void(int32_t, const rank_item*)>& afun)
		{
			
			if (m_time != m_pagetime)
			{
				init();
			}

			if (apage - 1 >= m_page.size())
			{
				return (int32_t)m_rankdata.size();
			}

			auto ltab = tab();
			int32_t lbegindex = (apage - 1) * ltab->m_everypagecount;
			int32_t lendindex = lbegindex + ltab->m_everypagecount;
			if (lbegindex < 0 || lbegindex > m_rankdata.size())
			{
				return (int32_t)m_rankdata.size();
			}
			auto itor = m_page[apage - 1];
			for (int lindex = 1; itor != m_rankdata.end() && lindex <= ltab->m_everypagecount; ++lindex, ++itor)
			{
				afun(lindex, *itor);
			}
			return (int32_t)m_rankdata.size();
		}

		void check()
		{
			int32_t lcount = count();
			if (lcount <= 0)
			{
				return;
			}
			while (lcount < m_rankdata.size())
			{
				m_rankdata.erase(std::prev(m_rankdata.end()));
			}
		}
	};
}//namespace ngl