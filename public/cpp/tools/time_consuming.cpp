#include "time_consuming.h"
#include "nprotocol.h"

namespace ngl
{
	time_consuming::time_consuming(const std::string& aname):
		m_name(aname),
		m_beg(0)
	{
	}

	void time_consuming::consuming_start()
	{
		m_beg = time_wheel::getms();
	}

	void time_consuming::consuming_finish()
	{
		int64_t lconsuming = time_wheel::getms() - m_beg;
		bool lerror = lconsuming > sysconfig::consumings();
		(lerror ? log_error() : log_info())->print("time consuming [{}:{}]", m_name, lconsuming);
	}
}