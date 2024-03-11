#include "threadtools.h"
#include <semaphore>

namespace ngl
{
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
		((type_sem*)m_sem)->acquire();
	}

	void sem::post()
	{
		((type_sem*)m_sem)->release();		
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