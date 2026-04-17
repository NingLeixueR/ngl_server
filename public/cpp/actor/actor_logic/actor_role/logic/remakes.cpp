/*
* Copyright (c) [2020-2025] NingLeixueR
* 
* Project name: ngl_server
* Project URL: https://github.com/NingLeixueR/ngl_server
* 
* This file is part of the ngl_server project and is distributed under the MIT License.
* You may use, modify, and distribute this project under the license, including commercial use,
* but you must retain the original copyright and license notice.
* 
* For license details, see the LICENSE file in the project root:
* https://github.com/NingLeixueR/ngl_server/blob/main/LICENSE
*/
// File overview: Implements player remark/note storage and retrieval.


#include "actor/actor_logic/actor_role/logic/remakes.h"
#include "actor/actor_logic/actor_role/actor_role.h"

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
		if (m_role != nullptr)
		{
			m_role->m_remakes.add_remakes(aremakes);
		}
	}

	dremakes::dremakes(actor_role* arole, const std::string& aremakes) :
		m_role(arole)
	{
		if (m_role != nullptr)
		{
			m_role->m_remakes.add_remakes(aremakes.c_str());
		}
	}

	dremakes::~dremakes()
	{
		if (m_role != nullptr)
		{
			m_role->m_remakes.erase_remakes();
		}
	}

	const char* dremakes::get_remake(actor_role* arole)
	{
		if (arole != nullptr)
		{
			return arole->m_remakes.get_remakes();
		}
		return "";
	}
}//namespace ngl