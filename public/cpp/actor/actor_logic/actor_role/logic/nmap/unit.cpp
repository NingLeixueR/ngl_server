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
#include "localtime.h"
#include "unit.h"

namespace ngl
{
	bool unit::set_type(eunit atype)
	{
		if (m_type != eunit_null)
		{
			log_error()->print("unit::set_type fail [{}:{}]", (int32_t)m_type, (int32_t)atype);
			return false;
		}
		m_type = atype;
		return true;
	}

	eunit unit::type()
	{
		return m_type;
	}

	attribute& unit::get_attribute()
	{
		return m_attribute;
	}

	bool unit::set_id(int32_t aid)
	{
		if (m_id != nguid::none_actordataid())
		{
			log_error()->print("unit::set_id fail [{}:{}]", m_id, aid);
			return false;
		}
		m_id = aid;
		return true;
	}

	int32_t unit::id()
	{
		return m_id;
	}

	dynamic_attribute& unit::dynamic()
	{
		return m_dynamic;
	}
}// namespace ngl