#pragma once

#include "time_wheel.h"
#include "nlog.h"

namespace ngl
{
	class time_consuming
	{
		time_consuming() = delete;
		time_consuming(const time_consuming&) = delete;
		time_consuming& operator=(const time_consuming&) = delete;

		std::string		m_name;
		int64_t			m_beg;
	public:
		time_consuming(const std::string& aname);
		~time_consuming();
	};
}//namespace ngl