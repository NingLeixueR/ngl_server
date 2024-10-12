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

		int32_t m_id;
	public:
		time_consuming(const std::string& aname, const std::function<bool(int64_t, int64_t)>& afun);
		~time_consuming();
	};
}//namespace ngl