#include "threadtools.h"
#include <boost/interprocess/sync/interprocess_semaphore.hpp>
#include <boost/thread.hpp>

namespace ngl
{
	using type_sem = boost::interprocess::interprocess_semaphore;

	sem::sem() :
		m_sem(new type_sem(0))
	{
	}

	sem::~sem()
	{
	}

	void sem::wait()
	{
		((type_sem*)m_sem)->wait();
	}
	void sem::post()
	{
		((type_sem*)m_sem)->post();
	}

	void sleep::seconds(int32_t avalue)
	{
		boost::this_thread::sleep(boost::posix_time::seconds(avalue));
	}

	void sleep::milliseconds(int32_t avalue)
	{
		boost::this_thread::sleep(boost::posix_time::milliseconds(avalue));
	}

	void sleep::hours(int32_t avalue)
	{
		boost::this_thread::sleep(boost::posix_time::hours(avalue));
	}
}