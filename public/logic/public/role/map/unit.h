#pragma once

#include "attribute.h"
#include "net.pb.h"

namespace ngl
{
	class unit
	{
		pbnet::eunit m_type;		
		attribute m_attribute;
		int64_t m_id;
		pbnet::UNIT_POSITION m_position;
		int64_t m_lastupdate;
	public:
		unit(pbnet::eunit atype, int64_t aid);

		attribute& get_attribute();

		int64_t id();
		pbnet::eunit type();

		int32_t x();
		int32_t y();
		int32_t angle();
		int32_t speed();

		void set_x(int32_t ax);
		void set_y(int32_t ay);
		void set_angle(int32_t aangle);
		void set_speed(int32_t aspeed);

		pbnet::UNIT_POSITION& position();
		void set_position_invalid();
		bool is_position_invalid();

		void update(int64_t alastupdate, pbnet::VECTOR2& apos);
	};

	class unit_role : public unit
	{
	public:
		unit_role(int64_t aid) :
			unit(pbnet::eunit_role, aid)
		{}
	};

	class unit_monster : public unit
	{
	public:
		unit_monster(int64_t aid) :
			unit(pbnet::eunit_monster, aid)
		{}
	};

	class unit_region : public unit
	{
	public:
		unit_region(int64_t aid) :
			unit(pbnet::eunit_region, aid)
		{}
	};
	
}