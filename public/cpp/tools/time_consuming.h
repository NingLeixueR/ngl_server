#pragma once

#include "time_wheel.h"
#include "nlog.h"

namespace ngl
{
	
	class time_consuming
	{
		std::string m_name;
		int64_t m_beg;
		int64_t m_end;
		std::function<bool(int64_t, int64_t)> m_fun;
	public:
		time_consuming(const std::string& aname, const std::function<bool(int64_t, int64_t)>& afun) :
			m_name(aname),
			m_beg(time_wheel::getms()),
			m_fun(afun)
		{
			LogLocalWarn("time consuming [%] start", aname);
		}

		~time_consuming()
		{
			m_end = time_wheel::getms();
			if (m_fun(m_beg, m_end) == false)
			{
				LogLocalWarn("time consuming [%] finish [%]-[%]=[%] ", m_end, m_beg, m_end - m_beg);
			}
			else
			{
				LogLocalError("time consuming [%] finish [%]-[%]=[%] ", m_end, m_beg, m_end - m_beg);
			}
		}
	};

}