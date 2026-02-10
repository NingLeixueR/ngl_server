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

#include "skill.h"
#include "xmlinfo.h"
#include "unit.h"

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
		// 检查玩家身上的状态
		if (!ldynamic.is_releaseskill())
		{
			return false;
		}

		// 冷却缩减 万分比
		int32_t lcoolabsolute = ldynamic.get_attribute(E_CooldownAbsolute);
		int32_t lcoolprop = ldynamic.get_attribute(E_CooldownProp);

		int32_t lcooldown = cooldown();

		int32_t lvalue = lcooldown - ((lcooldown / 10000) * lcoolprop) - lcoolabsolute;
		if (lvalue < 0)
		{
			lvalue = 0;
		}

		int32_t lnow = (int32_t)localtime::gettime();
		if (m_cooldownutc + lvalue > lnow)
		{
			return false;
		}

		return true;
	}

}//namespace ngl