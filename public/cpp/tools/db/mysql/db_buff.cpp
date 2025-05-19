#include "db_buff.h"

namespace ngl
{
	bool db_buff::malloc_function(ptr& aptr, const std::function<bool(ptr&)>& afun)
	{
		int32_t lpos = 0;
		if (!m_buffbyte.empty())
		{
			lpos = (m_buffbyte.rbegin()->first / m_buffsize) - 1;
		}

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
		aptr.free();
		int32_t llen = m_buffsize * (apos + 1);
		auto itor = m_buffbyte.find(llen);
		if (itor != m_buffbyte.end())
		{
			aptr.m_buff = itor->second;
			aptr.m_bufflen = itor->first;
			return;
		}
		aptr.m_buff = new char[llen];
		aptr.m_bufflen = llen;
		return;
	}

	void db_buff::free(ptr& aptr)
	{
		m_buffbyte[aptr.m_bufflen] = aptr.m_buff;
		aptr.m_buff = nullptr;
		aptr.m_bufflen = 0;
	}
}// namespace ngl