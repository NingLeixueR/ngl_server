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

#include "actor/actor_base/core/nactortype.h"
#include "tools/serialize/ndefine.h"
#include "tools/tools/tools_enum.h"
#include "tools/type.h"

#include <istream>
#include <ostream>
#include <format>

namespace ngl
{
	// Packed actor identifier:
	// - 16 bits actor type
	// - 16 bits area/shard
	// - 32 bits actor-local data id
	union nguid
	{
	private:
		static constexpr int16_t actor_value(ENUM_ACTOR avalue)
		{
			return static_cast<int16_t>(avalue);
		}

		int64_t m_id;
		int32_t m_value1[2]; // m_value1[0] stores type+area, m_value1[1] stores actordataid.
		int16_t m_value2[4]; // m_value2[0] stores type, m_value2[1] stores area.
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

		// Allow nguid to be passed anywhere an i64_actorid is expected.
		inline operator i64_actorid ()const
		{
			return m_id;
		}

		// Return the packed 64-bit representation.
		inline i64_actorid id()const
		{
			return m_id;
		}

		// Return the actor type portion.
		inline ENUM_ACTOR type()const
		{
			return static_cast<ENUM_ACTOR>(m_value2[0]);
		}

		// Return the actor-local numeric id portion.
		inline i32_actordataid actordataid()const
		{
			return m_value1[1];
		}

		// Return the area/shard portion.
		inline i16_area area()const
		{
			return m_value2[1];
		}

		// Return a debug-friendly actor type name for a packed id.
		static const char* name(i64_actorid aactorid)
		{
			nguid lguid(aactorid);
			const auto ltype = static_cast<ENUM_ACTOR>(lguid.type());
			return tools::em<ENUM_ACTOR>::name(ltype);
		}

		// Build a packed id from all three components.
		static i64_actorid make(ENUM_ACTOR atype, i16_area aareaid, i32_actordataid aid)
		{
			nguid lguid(atype, aareaid, aid);
			return (i64_actorid)lguid;
		}

		// Build an id when the actor has no concrete area.
		static i64_actorid make(ENUM_ACTOR atype, i32_actordataid aid)
		{
			nguid lguid(atype, none_area(), aid);
			return (i64_actorid)lguid;
		}

		// Build a type-only id used for singleton-style addressing.
		static i64_actorid make(ENUM_ACTOR atype)
		{
			nguid lguid(atype, none_area(), none_actordataid());
			return (i64_actorid)lguid;
		}

		// Build the sentinel "invalid actor id" value.
		static i64_actorid make()
		{
			nguid lguid(none_type(), none_area(), none_actordataid());
			return (i64_actorid)lguid;
		}

		// Build a type-only id in the local process area.
		static i64_actorid make_self(ENUM_ACTOR atype);

		// Return a copy with the type bits replaced.
		static i64_actorid make_type(i64_actorid aactorid, ENUM_ACTOR atype)
		{
			nguid lguid(aactorid);
			return lguid.make_type(atype);
		}

		// Return a copy with the area bits replaced.
		static i64_actorid make_area(i64_actorid aactorid, i16_area aareaid)
		{
			nguid lguid(aactorid);
			return lguid.make_area(aareaid);
		}

		// Return a copy with the data-id bits replaced.
		static i64_actorid make_actordataid(i64_actorid aactorid, i32_actordataid aid)
		{
			nguid lguid(aactorid);
			return lguid.make_actordataid(aid);
		}

		// Mutate the type bits in place.
		inline i64_actorid make_type(ENUM_ACTOR atype)
		{
			m_value2[0] = actor_value(atype);
			return (i64_actorid)(*this);
		}

		// Mutate the area bits in place.
		inline i64_actorid make_area(i16_area aareaid)
		{
			m_value2[1] = aareaid;
			return (i64_actorid)(*this);
		}

		// Mutate the data-id bits in place.
		inline i64_actorid make_actordataid(i32_actordataid aid)
		{
			m_value1[1] = aid;
			return (i64_actorid)(*this);
		}

		// Extract the data-id portion from a packed id.
		static i32_actordataid actordataid(i64_actorid aactorid)
		{
			nguid lguid(aactorid);
			return lguid.actordataid();
		}

		// Extract the actor type portion from a packed id.
		static i16_actortype type(i64_actorid aactorid)
		{
			nguid lguid(aactorid);
			return static_cast<i16_actortype>(lguid.type());
		}

		// Extract the area portion from a packed id.
		static i16_area area(i64_actorid aactorid)
		{
			nguid lguid(aactorid);
			return lguid.area();
		}

		// Build the "all actors of this type" broadcast id.
		static i64_actorid moreactor(ENUM_ACTOR atype)
		{
			return make(atype, none_area(), none_actordataid());
		}

		// Build the sentinel broadcast id for "all actor types".
		static i64_actorid moreactor()
		{
			return make(none_type(), none_area(), none_actordataid());
		}

		// Reset this object to the invalid sentinel value.
		inline void none()
		{
			m_value2[0] = actor_value(none_type());
			m_value2[1] = none_area();
			m_value1[1] = none_actordataid();
		}

		// Return whether the packed id represents a type-wide broadcast target.
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

		// Return whether the type portion is the sentinel invalid value.
		static bool is_actortypenone(i64_actorid actorid)
		{
			nguid lguid(actorid);
			return lguid.is_actortypenone();
		}

		inline bool is_actortypenone()const
		{
			return type() == none<i16_actortype>();
		}

		// Return whether the area portion is the sentinel invalid value.
		static bool is_actorareanone(i64_actorid actorid)
		{
			nguid lguid(actorid);
			return lguid.is_actorareanone();
		}

		// Return whether the area portion is the sentinel invalid value.
		inline bool is_actorareanone()const
		{
			return area() == none<i16_area>();
		}

		// Return whether the data-id portion is the sentinel invalid value.
		static bool is_actoridnone(i64_actorid actorid)
		{
			nguid lguid(actorid);
			return lguid.is_actoridnone();
		}

		// Return whether the data-id portion is the sentinel invalid value.
		inline bool is_actoridnone()const
		{
			return actordataid() == none<i32_actordataid>();
		}

		// Sentinel area value.
		static i16_area none_area()
		{
			return none<i16_area>();
		}

		// Sentinel actor type value.
		static ENUM_ACTOR none_type()
		{
			return none<ENUM_ACTOR>();
		}

		// Sentinel actor-local id value.
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
		const char* lanme = ngl::tools::em<ngl::ENUM_ACTOR>::name(aval.type());
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
