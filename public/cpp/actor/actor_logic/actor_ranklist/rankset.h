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
// File overview: Declares interfaces for actor ranklist.

#pragma once

#include "actor/actor_logic/actor_ranklist/rank_item.h"
#include "actor/tab/ttab_ranklist.h"
#include "actor/tab/ttab_activity.h"

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
		const tab_ranklist* m_tab;
	public:
		explicit rankset_base(pbdb::eranklist aranktype) :
			m_tab(ttab_ranklist::instance().tab(aranktype))
		{
			if (m_tab == nullptr)
			{
				tools::no_core_dump();
				return;
			}
		}
	
		inline const tab_ranklist* tab()
		{
			return m_tab;
		}

		inline int32_t showitem()
		{
			return m_tab->m_showitem;
		}

		inline int32_t everypagecount()
		{
			return m_tab->m_everypagecount;
		}

		inline int32_t maxitem()
		{
			return m_tab->m_maxitem;
		}

		inline int32_t minvalue()
		{
			return m_tab->m_minvalue;
		}		

		virtual void erase(rank_item* aitem) = 0;
		virtual void insert(rank_item* aitem) = 0;
		virtual void foreach(const std::function<void(int32_t, const rank_item*)>&) = 0;
		virtual int32_t getpage(i64_actorid aroleid, int32_t apage, const std::function<void(int32_t, const rank_item*)>& afun) = 0;
		virtual int32_t role_rank(i64_actorid aroleid) = 0;

	};

	template <pbdb::eranklist ETYPE>
	class rankset :
		public rankset_base
	{
		using setitor = std::set<rank_item*, operator_value<ETYPE>>::iterator;
		std::set<rank_item*, operator_value<ETYPE>> m_rankdata;
		int64_t							m_time = 0;			// Time
		std::vector<setitor>			m_page;				// Index
		int64_t							m_pagetime = 0;		// M_page time
		std::map<i64_actorid, int32_t>	m_rolerank;			// Key:roleid value:

		const tab_ranklist* tab()
		{
			auto ltab = ttab_ranklist::instance().tab(ETYPE);
			if (ltab == nullptr)
			{
				tools::no_core_dump();
				return nullptr;
			}
			return ltab;
		}

		void init()
		{
			m_page.clear();
			int32_t leverypagecount = everypagecount();
			int pagesize = showitem() / leverypagecount;
			m_page.resize(pagesize);

			auto itor = m_rankdata.begin();
			for (int i = 0; i < pagesize; ++i)
			{
				m_page[i] = itor;
				for (int j = 1; itor != m_rankdata.end() && j <= leverypagecount; ++itor,++j)
				{
					m_rolerank[(*itor)->m_actorid] = i * leverypagecount + j;
				}
			}
			m_pagetime = m_time;
		}
	public:
		rankset() :
			rankset_base(ETYPE)
		{}

		virtual void erase(rank_item* aitem)
		{
			m_rankdata.erase(aitem);
			m_time = localtime::gettime();
		}

		virtual void insert(rank_item* aitem)
		{
			if (aitem->value(ETYPE) < minvalue())
			{// To
				return;
			}
			m_rankdata.insert(aitem);
			while (m_rankdata.size() > maxitem())
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
				afun(++lindex, item);
			}
		}

		int32_t get_showcount()
		{
			int32_t lshow = showitem();
			int32_t lcount = (int32_t)m_rankdata.size();
			return lshow > lcount ? lcount : lshow;
		}

			virtual int32_t getpage([[maybe_unused]] i64_actorid aroleid, int32_t apage, const std::function<void(int32_t, const rank_item*)>& afun)
		{
			if (m_time != m_pagetime)
			{
				init();
			}

			if (apage - 1 >= m_page.size())
			{
				return get_showcount();
			}

			int32_t leverypagecount = everypagecount();
			int32_t lbegindex = (apage - 1) * leverypagecount;
				if (lbegindex < 0 || lbegindex > m_rankdata.size())
				{
					return get_showcount();
			}
			auto itor = m_page[apage - 1];
			for (int lindex = 1; itor != m_rankdata.end() && lindex <= leverypagecount; ++lindex, ++itor)
			{
				afun(lindex, *itor);
			}
			return get_showcount();
		}

		virtual int32_t role_rank(i64_actorid aroleid)
		{
			auto itor = m_rolerank.find(aroleid);
			if (itor == m_rolerank.end())
			{
				return -1;
			}
			return itor->second;
		}
	};

	class make_rank
	{
		template <pbdb::eranklist RANKTYPE, int32_t N>
		static std::unique_ptr<rankset_base> create_activity_rank()
		{
			return std::make_unique<rankset<(pbdb::eranklist)(RANKTYPE + N)>>();
		}
	public:
		static std::unique_ptr<rankset_base> make(pbdb::eranklist atype)
		{
				switch (static_cast<int>(atype))
				{
				case static_cast<int>(pbdb::eranklist::lv):
					return std::make_unique<rankset<pbdb::eranklist::lv>>();
				case static_cast<int>(pbdb::eranklist::gold):
					return std::make_unique<rankset<pbdb::eranklist::gold>>();
				case static_cast<int>(pbdb::eranklist::activity_lv) + 1:
					return create_activity_rank<pbdb::eranklist::activity_lv, 1>();
				case static_cast<int>(pbdb::eranklist::activity_gold) + 1:
					return create_activity_rank<pbdb::eranklist::activity_gold, 1>();
				}
			return nullptr;
		}
	};
}//namespace ngl
