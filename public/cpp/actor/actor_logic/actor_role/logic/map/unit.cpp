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
	unit::unit(pbnet::eunit atype, int64_t aid) :
		m_type(atype),
		m_id(aid),
		m_lastupdate(localtime::getms())
	{
		set_position_invalid();
	}

	attribute& unit::get_attribute()
	{
		return m_attribute;
	}

	int64_t unit::id()
	{
		return m_id;
	}

	pbnet::eunit unit::type()
	{
		return m_type;
	}

	int32_t unit::x()
	{
		return m_position.mposition().mx();
	}

	int32_t unit::y()
	{
		return m_position.mposition().my();
	}

	int32_t unit::angle()
	{
		return m_position.mangle();
	}

	int32_t unit::speed()
	{
		return m_position.mspeed();
	}

	void unit::set_x(int32_t ax)
	{
		m_position.mutable_mposition()->set_mx(ax);
	}

	void unit::set_y(int32_t ay)
	{
		m_position.mutable_mposition()->set_my(ay);
	}

	void unit::set_angle(int32_t aangle)
	{
		m_position.set_mangle(aangle);
	}

	void unit::set_speed(int32_t aspeed)
	{
		m_position.set_mspeed(aspeed);
	}

	pbnet::UNIT_POSITION& unit::position()
	{
		return m_position;
	}

	void unit::set_position_invalid()
	{
		m_position.mutable_mposition()->set_mx(-1);
		m_position.mutable_mposition()->set_my(-1);
	}

	bool unit::is_position_invalid()
	{
		return x() == -1 || y() == -1;
	}

	void unit::update(int64_t alastupdate, pbnet::VECTOR2& apos)
	{
		int64_t lvalue = alastupdate - m_lastupdate;
		apos.set_mx((int32_t)(x() + (speed() * cos(angle()) * lvalue)));
		apos.set_my((int32_t)(y() + (speed() * sin(angle()) * lvalue)));
	}
}// namespace ngl