#include "db_buff.h"

namespace ngl
{
	bool db_buff::malloc_function(const std::function<bool(std::shared_ptr<dbuff>&)>& afun)
	{
		for (int i = m_count; i < e_maxcount; ++i)
		{
			if (i > m_count)
			{
				m_count = i;
				m_buff = std::make_shared<dbuff>(e_buffsize * m_count);
			}
			if (afun(m_buff))
			{
				return true;
			}
		}
		return false;
	}
}// namespace ngl