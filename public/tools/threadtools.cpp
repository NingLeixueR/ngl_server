#include "threadtools.h"
//#include <boost/interprocess/sync/interprocess_semaphore.hpp>
//#include <boost/thread.hpp>
#include <semaphore>

namespace ngl
{
	//using type_sem = boost::interprocess::interprocess_semaphore;
	using type_sem = std::counting_semaphore<1>;

	sem::sem() :
		m_sem(new type_sem(0))
	{
	}

	sem::~sem()
	{
	}

	void sem::wait()
	{
		//((type_sem*)m_sem)->wait();
		((type_sem*)m_sem)->acquire();
	}
	void sem::post()
	{
		//((type_sem*)m_sem)->post();
		((type_sem*)m_sem)->release();		
	}

	void sleep::seconds(int32_t avalue)
	{
		//boost::this_thread::sleep(boost::posix_time::seconds(avalue));
		std::this_thread::sleep_for(std::chrono::seconds(avalue));
	}

	void sleep::milliseconds(int32_t avalue)
	{
		//boost::this_thread::sleep(boost::posix_time::milliseconds(avalue));
		std::this_thread::sleep_for(std::chrono::milliseconds(avalue));
	}

	void sleep::hours(int32_t avalue)
	{
		//boost::this_thread::sleep(boost::posix_time::hours(avalue));
		std::this_thread::sleep_for(std::chrono::hours(avalue));
	}
}