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
// File overview: Declares interfaces for actor base.

#pragma once

#include "actor/actor_base/nactortype.h"
#include "tools/serialize/ndefine.h"
#include "tools/enum2name.h"
#include "tools/type.h"

#include <format>
#include <istream>
#include <ostream>

namespace ngl
{
	///////////////////////////////////
	// Actor id(64bit) //
	// 16 TypeENUM_ACTOR //
	// 16 Areaid //
	// 32 Dataid //
	// ############ 64 bit ######### //
	// #actor_type###areaid###id#### //
	// #16bit########16bit####32bit# //
	///////////////////////////////////
	union nguid
	{
	private:
		static constexpr int16_t actor_value(ENUM_ACTOR avalue)
		{
			return static_cast<int16_t>(avalue);
		}

		int64_t m_id;
		int32_t m_value1[2];// m_value1[0] = type+area  m_value1[1]=actordataid
		int16_t m_value2[4];// m_value2[0] = type		m_value2[1] = area
	public:
		inline nguid()
		{
			m_value2[0] = actor_value(none_type());
			m_value2[1] = none_area();
			m_value1[1] = none_actordataid();
		}

		inline nguid(i64_actorid aid) :
			m_id(aid)
		{}

		inline nguid(ENUM_ACTOR atype, i16_area aareaid, i32_actordataid aid) :
			m_id(0)
		{
			m_value2[0] = actor_value(atype);
			m_value2[1] = aareaid;
			m_value1[1] = aid;
		}

		// # Can union i64_actorid
		inline operator i64_actorid ()const
		{
			return m_id;
		}

		// # Getguidid
		inline i64_actorid id()const
		{
			return m_id;
		}

		// # GetENUM_ACTORtype
		inline ENUM_ACTOR type()const
		{
			return static_cast<ENUM_ACTOR>(m_value2[0]);
		}

		// # Getdataid
		inline i32_actordataid actordataid()const
		{
			return m_value1[1];
		}

		// # Getarea
		inline i16_area area()const
		{
			return m_value2[1];
		}

		// # Throughnactortype::enum2nameget correspondingstring
		static const char* name(i64_actorid aactorid)
		{
			nguid lguid(aactorid);
			const auto ltype = static_cast<ENUM_ACTOR>(lguid.type());
			return em<ENUM_ACTOR>::name(ltype);
		}

		// # Type, area, dataidcreate i64_actorid
		static i64_actorid make(ENUM_ACTOR atype, i16_area aareaid, i32_actordataid aid)
		{
			nguid lguid(atype, aareaid, aid);
			return (i64_actorid)lguid;
		}

		// # Type, dataid, area none_area()create i64_actorid
		static i64_actorid make(ENUM_ACTOR atype, i32_actordataid aid)
		{
			nguid lguid(atype, none_area(), aid);
			return (i64_actorid)lguid;
		}

		// # Type, dataid none_actordataid(), area none_area()create i64_actorid
		static i64_actorid make(ENUM_ACTOR atype)
		{
			nguid lguid(atype, none_area(), none_actordataid());
			return (i64_actorid)lguid;
		}

		// # Typenone_type(), area none_area()dataid none_actordataid()create i64_actorid
		static i64_actorid make()
		{
			nguid lguid(none_type(), none_area(), none_actordataid());
			return (i64_actorid)lguid;
		}

		// # Type, area tab_self_area none_actordataid(), dataid none_actordataidcreate i64_actorid
		static i64_actorid make_self(ENUM_ACTOR atype);

		// # Replacetype
		static i64_actorid make_type(i64_actorid aactorid, ENUM_ACTOR atype)
		{
			nguid lguid(aactorid);
			return lguid.make_type(atype);
		}

		// # Replacearea
		static i64_actorid make_area(i64_actorid aactorid, i16_area aareaid)
		{
			nguid lguid(aactorid);
			return lguid.make_area(aareaid);
		}

		// # Replaceactordataid
		static i64_actorid make_actordataid(i64_actorid aactorid, i32_actordataid aid)
		{
			nguid lguid(aactorid);
			return lguid.make_actordataid(aid);
		}

		// # Replacetype
		inline i64_actorid make_type(ENUM_ACTOR atype)
		{
			m_value2[0] = actor_value(atype);
			return (i64_actorid)(*this);
		}

		// # Replacearea
		inline i64_actorid make_area(i16_area aareaid)
		{
			m_value2[1] = aareaid;
			return (i64_actorid)(*this);
		}

		// # Replaceactordataid
		inline i64_actorid make_actordataid(i32_actordataid aid)
		{
			m_value1[1] = aid;
			return (i64_actorid)(*this);
		}

		// # Getactordataid
		static i32_actordataid actordataid(i64_actorid aactorid)
		{
			nguid lguid(aactorid);
			return lguid.actordataid();
		}

		// # Gettype
		static i16_actortype type(i64_actorid aactorid)
		{
			nguid lguid(aactorid);
			return static_cast<i16_actortype>(lguid.type());
		}

		// # Getarea
		static i16_area area(i64_actorid aactorid)
		{
			nguid lguid(aactorid);
			return lguid.area();
		}

		// # Send sendto type allactor
		static i64_actorid moreactor(ENUM_ACTOR atype)
		{
			return make(atype, none_area(), none_actordataid());
		}

		// # Make()consistent
		static i64_actorid moreactor()
		{
			return make(none_type(), none_area(), none_actordataid());
		}

		// # Make()consistent
		inline void none()
		{
			m_value2[0] = actor_value(none_type());
			m_value2[1] = none_area();
			m_value1[1] = none_actordataid();
		}

		// # Whether to type allactor
		static bool is_moreactor(i64_actorid actorid, ENUM_ACTOR atype)
		{
			nguid lguid(actorid);
			return lguid.is_moreactor(atype);
		}

		inline bool is_moreactor(ENUM_ACTOR atype)const
		{
			nguid lguid(atype, none_area(), none_actordataid());
			return (i64_actorid)(lguid) == (i64_actorid)(*this);
		}

		// # Actor type whetherinvalid
		static bool is_actortypenone(i64_actorid actorid)
		{
			nguid lguid(actorid);
			return lguid.is_actortypenone();
		}

		inline bool is_actortypenone()const
		{
			return type() == none<i16_actortype>();
		}

		// # Actor area whetherinvalid
		static bool is_actorareanone(i64_actorid actorid)
		{
			nguid lguid(actorid);
			return lguid.is_actorareanone();
		}

		// # Actor area whetherinvalid
		inline bool is_actorareanone()const
		{
			return area() == none<i16_area>();
		}

		// # Actor id whetherinvalid
		static bool is_actoridnone(i64_actorid actorid)
		{
			nguid lguid(actorid);
			return lguid.is_actoridnone();
		}

		// # Actor id whetherinvalid
		inline bool is_actoridnone()const
		{
			return actordataid() == none<i32_actordataid>();
		}

		// # Invalidarea
		static i16_area none_area()
		{
			return none<i16_area>();
		}

		// # Invalidtype
		static ENUM_ACTOR none_type()
		{
			return none<ENUM_ACTOR>();
		}

		// # Invaliddataid
		static i32_actordataid none_actordataid()
		{
			return none<i32_actordataid>();
		}

		template <typename T>
		static T none()
		{
			return (T)(-1);
		}

		inline auto operator<=>(const nguid& r)const
		{
			return m_id <=> r.m_id;
		}

		DPROTOCOL(nguid, m_id)
	};
}//namespace ngl

namespace ngl
{
	inline std::ostream& operator<<(std::ostream& astream, const nguid& avalue)
	{
		astream << static_cast<i64_actorid>(avalue);
		return astream;
	}

	inline std::istream& operator>>(std::istream& astream, nguid& avalue)
	{
		i64_actorid lid = 0;
		if (astream >> lid)
		{
			avalue = nguid(lid);
		}
		return astream;
	}
}//namespace ngl

template <>
struct std::formatter<ngl::nguid>
{
	constexpr auto parse(const std::format_parse_context& ctx)const
	{
		return ctx.begin();
	}

	auto format(const ngl::nguid& aval, std::format_context& ctx)const
	{
		const char* lanme = ngl::em<ngl::ENUM_ACTOR>::name(aval.type());
		if (lanme == nullptr)
		{
			return std::format_to(ctx.out(), "guid<type:{} area:{} id:{}>", (int)aval.type(), aval.area(), aval.actordataid());
		}
		else
		{
			return std::format_to(ctx.out(), "guid<type:{} area:{} id:{}>", lanme, aval.area(), aval.actordataid());
		}
	}
};