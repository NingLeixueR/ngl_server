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
// File overview: Declares interfaces for attribute.

#pragma once

#include "actor/actor_logic/actor_role/logic/attribute/attribute.h"
#include "actor/pb/net.pb.h"
#include "actor/pb/db.pb.h"

namespace ngl
{
	class unit
	{
		unit() = delete;

		pbdb::EUNIT					m_type = pbdb::EUNIT::EUNIT_FAIL;
		i64_actorid					m_id = nguid::make();
		attribute					m_attribute;
		dynamic_attribute			m_dynamic;
		pbdb::POSITION				m_position;
	public:
		unit(i64_actorid aunitid, pbdb::EUNIT atype) :
			m_type(atype),
			m_dynamic(aunitid)
		{}

		void init(attribute aattribute)
		{
			m_attribute.get_attribute();
			m_dynamic.init(m_attribute.get_attribute());
		}

		// # [Set/get]unittype
		bool set_type(pbdb::EUNIT atype);
		pbdb::EUNIT type();

		// # Getunitattribute
		attribute& get_attribute();

		// # [Set/get]unit id
		bool set_id(int32_t aid);
		int32_t id();

		// # Getunit attribute
		dynamic_attribute& dynamic();

		// # [Set/get]unit position
		void set_x(int32_t ax);
		void set_y(int32_t ay);
		int32_t x();
		int32_t y();
		pbdb::POSITION& position();
		bool is_position_invalid();

		// # [Set/get]unit direction
		void set_angle(int32_t aangle);
		int32_t angle();

		// # [Set/get]unit
		void set_speed(int32_t aspeed);
		int32_t speed();

		void update(int64_t ams);
	};

	class unit_role: public unit
	{
	public:
		unit_role(i64_actorid aunitid) :
			unit(aunitid, pbdb::EUNIT::EUNIT_ROLE)
		{}
	};

	class unit_monster : public unit
	{
	public:
		unit_monster(i64_actorid aunitid):
			unit(aunitid, pbdb::EUNIT::EUNIT_MONSTER)
		{}
	};
}// namespace ngl