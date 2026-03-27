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
// File overview: Implements logic for attribute.


#include "actor/actor_logic/actor_role/logic/attribute/skill.h"
#include "actor/actor_logic/actor_role/logic/attribute/unit.h"
#include "tools/tab/xml/xmlinfo.h"

namespace ngl
{
	bool skill::set_lv(int32_t askilllv)
	{
		if (askilllv <= 0)
		{
			log_error()->print("skill::set_lv({}) fail", askilllv);
			return false;
		}
		m_lv = askilllv;
		return true;
	}

	int32_t skill::lv()
	{
		return m_lv;
	}

	int32_t skill::cooldown()
	{
		auto itor = m_template.m_cooldown.find(m_lv);
		if (itor == m_template.m_cooldown.end())
		{
			return -1;
		}
		return itor->second;
	}

	bool skill::release_check()
	{
		dynamic_attribute& ldynamic = m_unit->dynamic();
		// Player on state
		if (!ldynamic.is_releaseskill())
		{
			return false;
		}

		// Translated comment.
		int32_t lcoolabsolute = static_cast<int32_t>(ldynamic.get_attribute(E_CooldownAbsolute));
		int32_t lcoolprop = static_cast<int32_t>(ldynamic.get_attribute(E_CooldownProp));

		int32_t lcooldown = cooldown();

		int32_t lvalue = lcooldown - ((lcooldown / 10000) * lcoolprop) - lcoolabsolute;
		if (lvalue < 0)
		{
			lvalue = 0;
		}

		int32_t lnow = static_cast<int32_t>(tools::localtime::gettime());
		if (m_cooldownutc + lvalue > lnow)
		{
			return false;
		}

		return true;
	}

}//namespace ngl
