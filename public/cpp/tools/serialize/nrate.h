#pragma once

#include "tools.h"

namespace ngl
{
	class nrate
	{
		struct interval
		{
			int m_resetutc = 0;		// 上次重置的utc
			int m_ratecount = 0;	// 收到数据包的数量

			interval() :
				m_resetutc((int)localtime::gettime()), m_ratecount(0)
			{}

			void reset()
			{
				m_resetutc = (int)localtime::gettime();
				m_ratecount = 0;
			}
		};
		std::map<i32_socket, interval> m_data;
	public:
		bool add(i32_socket aid)
		{
			int lnow = (int)localtime::gettime();
			auto lpinterval = tools::findmap(m_data, aid);
			if (lpinterval == nullptr)
			{
				m_data.insert(std::make_pair(aid, interval()));
				return true;
			}
			if (lpinterval->m_resetutc == 0)
			{
				lpinterval->reset();
				return true;
			}
			if (lnow >= lpinterval->m_resetutc + sysconfig::rate_interval())
			{
				lpinterval->reset();
				return true;
			}
			++lpinterval->m_ratecount;
			if (sysconfig::rate_count() < lpinterval->m_ratecount)
			{
				return false;
			}
			return true;
		}
	};
}//namespace ngl