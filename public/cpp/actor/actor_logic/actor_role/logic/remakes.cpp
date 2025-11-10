/*
* Copyright (c) [2020-2025] NingLeixueR
* 
* 项目名称：ngl_server
* 项目地址：https://github.com/NingLeixueR/ngl_server
* 
* 本文件是 ngl_server 项目的一部分，遵循 MIT 开源协议发布。
* 您可以按照协议规定自由使用、修改和分发本项目，包括商业用途，
* 但需保留原始版权和许可声明。
* 
* 许可详情参见项目根目录下的 LICENSE 文件：
* https://github.com/NingLeixueR/ngl_server/blob/main/LICENSE
*/
#include "actor_role.h"
#include "remakes.h"

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