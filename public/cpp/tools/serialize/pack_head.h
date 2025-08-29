#pragma once

#include "nserialize.h"
#include "csvtable.h"
#include "tools.h"
#include "type.h"

#include <cstdint>
#include <map>

namespace ngl
{
	struct pack_head
	{
		int32_t m_data[EPH_SUM] = {0};
		int32_t m_wpos = 0;

		pack_head();

		// # 设置mask
		static void				head_set_mask(int32_t* abuff);
		static EPH_HEAD_VAL		head_check_mask(const int32_t* abuff, int awpos);
		void					set_mask();
		EPH_HEAD_VAL			check_mask()const;

		// # 给包头设置/获取时间
		static void		head_set_time(int32_t* abuff);
		static i32_time head_get_time(const int32_t* abuff);

		// # 设置时间戳
		void			set_time();

		// # 获取时间戳
		i32_time		get_time()const;

		// # 设置actor 
		// # aactor:发送给哪个actor /* aenum == ACTOR_NONE 此值无效 */
		// # arequestactorid:哪个actor发送的
		static void head_set_actor(int32_t* abuff, i64_actorid aactor, i64_actorid arequestactorid);
		static void head_set_actor(int32_t* abuff, i64_actorid aactor);
		void set_actor(i64_actorid aactor, i64_actorid arequestactorid);
		void set_requestactor(i64_actorid arequestactorid);

		// # 获取要发送给哪个actor
		i64_actorid		get_actor()const;

		// # 获取谁发送的actor
		i64_actorid		get_request_actor()const;

		// # 获取要发送给的actor的type
		i16_actortype	get_actortype()const;

		// # 获取谁发送的actor的type
		i16_actortype	get_request_actortype()const;

		// # 获取要发送给的actor的area
		i16_area		get_actorarea()const;

		// # 获取要发送给的actor的dataid
		i32_actordataid get_actordataid()const;

		// # 重置head
		void			reset();

		// # 获取EPH对应的值
		int32_t			getvalue(EPH aeph)const;

		// # 获取协议字节数
		int32_t			get_bytes()const;

		// # 获取包头长度
		static int32_t	size();

		// # 包头是否接收完毕
		EPH_HEAD_VAL	isready()const;

		// # 获取协议号
		i32_protocolnum protocolnum()const;

		// # 获取/设置协议号
		i32_protocolnum get_protocolnumber()const;
		void			set_protocol(i32_protocolnum aprotocolnum);
		
		EPH_HEAD_VAL push(const char*& abuff, int32_t& alen);
		bool push_format(ngl::ser::serialize_push* aserialize)const
		{
			return aserialize->basetype((void*)m_data, sizeof(int32_t) * EPH_SUM);
		}

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
		ngl::nguid lactor(*(int64_t*)&val.m_data[ngl::EPH_ACTOR_TYPEAREA]);
		ngl::nguid lrequestactor(*(int64_t*)&val.m_data[ngl::EPH_REQUEST_ACTOR_TYPEAREA]);
		std::format_to(out, "HEAD[MASK:{},BYTES:{},TIME:{},PROTOCOLNUM:{},ACTOR:{},REQUEST_ACTOR:{}]", 
			ss, val.m_data[ngl::EPH_BYTES],
			ngl::localtime::time2str(val.m_data[ngl::EPH_TIME], "%y/%m/%d %H:%M:%S"),
			val.m_data[ngl::EPH_PROTOCOLNUM], lactor, lrequestactor
		);
		return out;
	}
};