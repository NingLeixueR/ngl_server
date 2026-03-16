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
// File overview: Declares interfaces for serialize.

#pragma once

#include "tools/serialize/nserialize.h"
#include "actor/actor_base/nguid.h"
#include "actor/tab/csvtable.h"
#include "tools/localtime.h"
#include "tools/tools.h"
#include "tools/type.h"

#include <algorithm>
#include <cstdint>
#include <format>
#include <map>
#include <string>

namespace ngl
{
	// # Packet header
	struct pack_head
	{
		int32_t m_data[EPH_SUM] = {0};
		int32_t m_wpos = 0;

		pack_head();

		// # Setmask
		static void				head_set_mask(int32_t* abuff);
		void					set_mask();

		// # Mask
		static EPH_HEAD_VAL		head_check_mask(const int32_t* abuff, int awpos);
		EPH_HEAD_VAL			check_mask()const;

		// # Setpacket headertime
		static void		head_set_time(int32_t* abuff);
		void			set_time();

		// # Getpacket headertime
		static i32_time head_get_time(const int32_t* abuff);
		i32_time		get_time()const;

		// # Setactor
		// # Aactor:sendto actor /* aenum == ACTOR_NONE this invalid */
		// # Arequestactorid: actorsend
		static void head_set_actor(int32_t* abuff, i64_actorid aactor, i64_actorid arequestactorid);
		static void head_set_actor(int32_t* abuff, i64_actorid aactor);
		void		set_actor(i64_actorid aactor, i64_actorid arequestactorid);
		void		set_requestactor(i64_actorid arequestactorid);

		// # Getactor
		i64_actorid		get_actor()const;
		i64_actorid		get_request_actor()const;

		// # Get sendto actor type
		i16_actortype	get_actortype()const;

		// # Get send actor type
		i16_actortype	get_request_actortype()const;

		// # Get sendto actor area
		i16_area		get_actorarea()const;

		// # Get sendto actor dataid
		i32_actordataid get_actordataid()const;

		// # Head
		void			reset();

		// # GetEPHcorresponding
		int32_t			getvalue(EPH aeph)const;

		// # Getprotocol
		int32_t			get_bytes()const;

		// # Getpacket header
		static int32_t	size();

		// # Packet headerwhether
		EPH_HEAD_VAL	isready()const;

		// # Getprotocol id
		i32_protocolnum protocolnum()const;

		// # Getprotocol id
		i32_protocolnum get_protocolnumber()const;

		// # Setprotocol id
		void set_protocol(i32_protocolnum aprotocolnum);
		
		EPH_HEAD_VAL push(const char*& abuff, int32_t& alen);

		bool push_format(ngl::ser::serialize_push* aserialize)const;

		void reservebuff(char* abuff, int abufflen, std::pair<char*, int32_t>& apair);
	};
}// namespace ngl

template <>
struct std::formatter<ngl::pack_head>
{
	constexpr auto parse(std::format_parse_context& ctx)
	{
		return ctx.begin();
	}

	template <typename FormatContext>
	auto format(const ngl::pack_head& val, FormatContext& ctx)const
	{
		auto out = ctx.out();
		std::string ss;
		const int8_t* lparr = (const int8_t*)&val.m_data[ngl::EPH_MASK];
		std::for_each(lparr, lparr + sizeof(int32_t) * ngl::EPH_MASK_COUNT, [&ss](int8_t avalue)
			{
				ss += std::format("{} ", avalue);
			});
		ngl::nguid lactor(val.get_actor());
		ngl::nguid lrequestactor(val.get_request_actor());
		std::format_to(out
			, R"(
HEAD[
	mask:{}
	bytes:{}
	time:{}
	protocol:{}
	actor:{}
	request_actor:{}
])"
			/*mask*/			, ss
			/*bytes*/			, val.m_data[ngl::EPH_BYTES]
			/*time*/			, ngl::localtime::time2str(val.m_data[ngl::EPH_TIME], "%y/%m/%d %H:%M:%S")
			/*protocol*/		, val.m_data[ngl::EPH_PROTOCOLNUM]
			/*actor*/			, lactor
			/*request_actor*/	, lrequestactor
		);
		return out;
	}
};