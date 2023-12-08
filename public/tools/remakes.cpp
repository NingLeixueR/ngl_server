#include "remakes.h"
#include "actor_role.h"

namespace ngl
{
	void remakes::add_remakes(const char* aremakes)
	{
		m_remakes.push(aremakes);
	}

	void remakes::erase_remakes()
	{
		if (m_remakes.empty() == false)
		{
			m_remakes.pop();
		}
	}

	const char* remakes::get_remakes()
	{
		if (m_remakes.empty() == false)
		{
			return m_remakes.top().c_str();
		}
		return "";
	}

	dremakes::dremakes(actor_role* arole, const char* aremakes):
		m_role(arole)
	{
		m_role->m_remakes.add_remakes(aremakes);
	}

	dremakes::dremakes(actor_role* arole, const std::string& aremakes) :
		dremakes(arole, aremakes.c_str())
	{}

	dremakes::~dremakes()
	{
		m_role->m_remakes.erase_remakes();
	}

	const char* dremakes::get_remake(actor_role* arole)
	{
		return arole->m_remakes.get_remakes();
	}
}