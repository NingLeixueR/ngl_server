#include "unit.h"

namespace ngl
{
	unit::unit(pbnet::eunit atype, int64_t aid) :
		m_type(atype),
		m_id(aid)
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
		return m_position.m_position().m_x();
	}

	int32_t unit::y()
	{
		return m_position.m_position().m_y();
	}

	int32_t unit::angle()
	{
		return m_position.m_angle();
	}

	int32_t unit::speed()
	{
		return m_position.m_speed();
	}

	void unit::set_x(int32_t ax)
	{
		m_position.mutable_m_position()->set_m_x(ax);
	}

	void unit::set_y(int32_t ay)
	{
		m_position.mutable_m_position()->set_m_y(ay);
	}

	void unit::set_angle(int32_t aangle)
	{
		m_position.set_m_angle(aangle);
	}

	void unit::set_speed(int32_t aspeed)
	{
		m_position.set_m_speed(aspeed);
	}

	pbnet::UNIT_POSITION& unit::position()
	{
		return m_position;
	}

	void unit::set_position_invalid()
	{
		m_position.mutable_m_position()->set_m_x(-1);
		m_position.mutable_m_position()->set_m_y(-1);
	}

	bool unit::is_position_invalid()
	{
		return x() == -1 || y() == -1;
	}

	void unit::update(int64_t alastupdate, pbnet::VECTOR2& apos)
	{
		int64_t lvalue = alastupdate - m_lastupdate;
		apos.set_m_x(x() + (speed() * cos(angle()) * lvalue));
		apos.set_m_y(y() + (speed() * sin(angle()) * lvalue));
	}
}