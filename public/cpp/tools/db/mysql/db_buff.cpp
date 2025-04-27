#include "db_buff.h"

namespace ngl
{
	bool db_buff::malloc_function(ptr& aptr, const std::function<bool(ptr&)>& afun)
	{
		int32_t lpos = 0;
		while (lpos < m_buffcount)
		{
			malloc(aptr, lpos);
			if (afun(aptr) == false)
			{
				++lpos;
				continue;
			}
			else
			{
				return true;
			}
		}
		return false;
	}
	
	void db_buff::malloc(ptr& aptr, int32_t apos)
	{
		int32_t llen = m_buffsize * (apos + 1);
		if (m_buffcount > apos)
		{
			aptr.free();
			for (auto itor = m_bufflist.begin(); itor != m_bufflist.end(); ++itor)
			{
				if (itor->second >= llen)
				{
					aptr.m_buff = itor->first;
					aptr.m_bufflen = itor->second;
					m_bufflist.pop_front();
					return;
				}
			}
		}
		aptr.m_buff = new char[llen];
		aptr.m_bufflen = llen;
		return;
	}

	void db_buff::free(ptr& aptr)
	{
		m_bufflist.push_back(std::make_pair(aptr.m_buff, aptr.m_bufflen));
		aptr.m_buff = nullptr;
		aptr.m_bufflen = 0;
	}
}// namespace ngl