#include "threadtools.h"
#include <semaphore>

namespace ngl
{
	sem::sem() :
		m_sem(new std::counting_semaphore<1>(0))
	{
	}

	sem::~sem()
	{
		delete m_sem;
	}

	void sem::wait()
	{
		((std::counting_semaphore<1>*)m_sem)->acquire();
	}

	void sem::post()
	{
		((std::counting_semaphore<1>*)m_sem)->release();
	}

	void sleep::seconds(int32_t avalue)
	{
		std::this_thread::sleep_for(std::chrono::seconds(avalue));
	}

	void sleep::milliseconds(int32_t avalue)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(avalue));
	}

	void sleep::hours(int32_t avalue)
	{
		std::this_thread::sleep_for(std::chrono::hours(avalue));
	}
}// namespace ngl