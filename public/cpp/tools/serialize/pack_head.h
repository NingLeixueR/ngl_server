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
// File overview: Packet header layout defining framing fields for the binary protocol.

#pragma once

#include "actor/actor_base/core/nguid.h"
#include "tools/serialize/nserialize.h"
#include "tools/tools/tools_time.h"
#include "actor/tab/csvtable.h"
#include "tools/tools.h"
#include "tools/type.h"

#include <algorithm>
#include <cstdint>
#include <format>
#include <string>
#include <map>

namespace ngl
{
	// Fixed-size wire header prepended to every framed TCP/KCP packet.
	struct pack_head
	{
		// The header is stored as raw int32 slots so it can be copied directly
		// into network buffers without an extra serialization layer.
		int32_t m_data[EPH_SUM] = {0};
		int32_t m_wpos = 0;

		pack_head();

		// A constant byte mask prefixes every packet and lets the receiver
		// distinguish framed traffic from ad-hoc telnet commands.
		static void				head_set_mask(int32_t* abuff);
		void					set_mask();

		// Validate the framing mask while the header is being streamed in.
		static EPH_HEAD_VAL		head_check_mask(const int32_t* abuff, int awpos);
		EPH_HEAD_VAL			check_mask()const;

		// Stamp the sender's current logical time into the header.
		static void		head_set_time(int32_t* abuff);
		void			set_time();

		// Read the sender timestamp back from the header.
		static i32_time head_get_time(const int32_t* abuff);
		i32_time		get_time()const;

		// Write destination/source actor ids into the header.
		static void head_set_actor(int32_t* abuff, i64_actorid aactor, i64_actorid arequestactorid);
		static void head_set_actor(int32_t* abuff, i64_actorid aactor);
		void		set_actor(i64_actorid aactor, i64_actorid arequestactorid);
		void		set_requestactor(i64_actorid arequestactorid);

		// Read destination/source actor ids from the header.
		i64_actorid		get_actor()const;
		i64_actorid		get_request_actor()const;

		// Convenience accessors for the packed destination actor guid.
		i16_actortype	get_actortype()const;

		// Convenience accessor for the packed source actor guid.
		i16_actortype	get_request_actortype()const;

		// Destination actor area/shard.
		i16_area		get_actorarea()const;

		// Destination actor-local numeric id.
		i32_actordataid get_actordataid()const;

		// Clears the partial read state and every header field.
		void			reset();

		// Raw slot accessor used by framing code.
		int32_t			getvalue(EPH aeph)const;

		// Declared payload byte count.
		int32_t			get_bytes()const;

		// Returns the fixed serialized header size in bytes.
		static int32_t	size();

		// Reports whether the mask/header is complete, still partial, or invalid.
		EPH_HEAD_VAL	isready()const;

		// Protocol number stored in the header.
		i32_protocolnum protocolnum()const;

		// Alias kept for older call sites.
		i32_protocolnum get_protocolnumber()const;

		// Set the protocol number stored in the header.
		void set_protocol(i32_protocolnum aprotocolnum);
		
		EPH_HEAD_VAL push(const char*& abuff, int32_t& alen);

		bool push_format(ngl::ser::serialize_push* aserialize)const;

		// Skips the header prefix when callers need the writable payload span.
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
			/*time*/			, ngl::tools::time::time2str(val.m_data[ngl::EPH_TIME], "%y/%m/%d %H:%M:%S")
			/*protocol*/		, val.m_data[ngl::EPH_PROTOCOLNUM]
			/*actor*/			, lactor
			/*request_actor*/	, lrequestactor
		);
		return out;
	}
};
