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