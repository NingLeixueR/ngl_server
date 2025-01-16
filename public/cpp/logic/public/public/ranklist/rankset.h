#pragma once

#include "rank_item.h"

namespace ngl
{
	template <pbdb::eranklist RTYPE>
	class operator_value
	{
	public:
		// >
		bool operator()(rank_item* al, rank_item* ar)const
		{
			return al->bigger(RTYPE, *ar);
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
		virtual int32_t getpage(int32_t apage, int32_t aevernmu, const std::function<void(int32_t, const rank_item*)>& afun) = 0;
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
	class rankset :public rankset_base
	{
		std::set<rank_item*, operator_value<ETYPE>> m_rankdata;
	public:
		virtual void erase(rank_item* aitem)
		{
			m_rankdata.erase(aitem);
		}

		virtual void insert(rank_item* aitem)
		{
			m_rankdata.insert(aitem);
		}

		virtual void foreach(const std::function<void(int32_t, const rank_item*)>& afun)
		{
			int32_t lindex = 0;
			for (const rank_item* item : m_rankdata)
			{
				afun(lindex++, item);
			}
		}

		virtual int32_t getpage(int32_t apage, int32_t aevernmu, const std::function<void(int32_t, const rank_item*)>& afun)
		{
			int32_t lbegindex = (apage - 1) * aevernmu;
			int32_t lendindex = lbegindex + aevernmu;
			if (lbegindex < 0 || lbegindex > m_rankdata.size())
			{
				return (int32_t)m_rankdata.size();
			}
			auto itor = m_rankdata.begin();
			std::advance(itor, lbegindex);
			for (int lindex = 1; itor != m_rankdata.end() && lindex <= aevernmu; ++lindex, ++itor)
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