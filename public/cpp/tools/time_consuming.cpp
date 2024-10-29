#include "time_consuming.h"
#include "nprotocol.h"

namespace ngl
{
	time_consuming::time_consuming(const std::string& aname):
		m_name(aname),
		m_beg(time_wheel::getms())
	{
	}

	time_consuming::~time_consuming()
	{
		try
		{
			int64_t lconsuming = time_wheel::getms() - m_beg;
			bool lerror = lconsuming > sysconfig::consumings();
			(lerror ? log_error():log_info())->print("time consuming [{}] [{}]", m_name, lconsuming);
		}
		catch (...)
		{
		}		
	}
}