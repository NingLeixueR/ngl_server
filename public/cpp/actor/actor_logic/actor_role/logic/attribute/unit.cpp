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


#include "actor/actor_logic/actor_role/logic/attribute/unit.h"
#include "tools/localtime.h"

namespace ngl
{
	bool unit::set_type(pbdb::EUNIT atype)
	{
		if (m_type != pbdb::EUNIT::EUNIT_FAIL)
		{
			log_error()->print("unit::set_type fail [{}:{}]", (int32_t)m_type, (int32_t)atype);
			return false;
		}
		m_type = atype;
		return true;
	}

	pbdb::EUNIT unit::type()
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

	void unit::set_x(int32_t ax)
	{
		m_position.mutable_mposition()->set_mx(ax);
	}

	void unit::set_y(int32_t ay)
	{
		m_position.mutable_mposition()->set_my(ay);
	}

	int32_t unit::x()
	{
		return m_position.mposition().mx();
	}

	int32_t unit::y()
	{
		return m_position.mposition().my();
	}

	pbdb::POSITION& unit::position()
	{
		return m_position;
	}

	bool unit::is_position_invalid()
	{
		if (m_position.mmapid() <= 0)
		{
			return true;
		}
		if (x() <= 0)
		{
			return true;
		}
		if (y() <= 0)
		{
			return true;
		}
		return false;
	}

	void unit::set_angle(int32_t aangle)
	{
		m_position.set_mangle(aangle);
	}

	int32_t unit::angle()
	{
		return m_position.mangle();
	}

	void unit::set_speed(int32_t aspeed)
	{
		m_position.set_mspeed(aspeed);
	}

	int32_t unit::speed()
	{
		return m_position.mspeed();
	}

	void unit::update(int64_t ams)
	{

	}
}// namespace ngl