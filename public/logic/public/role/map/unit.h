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
	public:
		unit(pbnet::eunit atype, int64_t aid);

		attribute& get_attribute();

		int64_t id();
		pbnet::eunit type();

		int32_t x();
		int32_t y();
		void set_x(int32_t ax);
		void set_y(int32_t ay);
		pbnet::UNIT_POSITION& position();
		void set_position_invalid();
		bool is_position_invalid();
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