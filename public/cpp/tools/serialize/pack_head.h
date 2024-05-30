#pragma once

#include "serialize.h"
#include "csvtable.h"
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

		// # 给包头设置或者获取时间
		static void		head_set_time(int32_t* abuff);
		static i32_time head_get_time(const int32_t* abuff);
		void			set_time();
		i32_time		get_time()const;

		static void		head_set_version(int32_t* abuff);
		static int32_t  head_get_version(const int32_t* abuff);
		void			set_version();
		int32_t			get_version()const;

		// # 设置actor 
		// # aactor:发送给哪个actor 
		// # arequestactorid:哪个actor发送的
		static void head_set_actor(
			int32_t* abuff
			, i64_actorid aactor	/* aenum == ACTOR_NONE 此值无效 */
			, i64_actorid arequestactorid
		);

		void set_actor(
			i64_actorid aactor		/* aenum == ACTOR_NONE 此值无效 */
			, i64_actorid arequestactorid
		);

		void set_requestactor(i64_actorid arequestactorid);

		static void set_actor(uint32_t* abuff, i64_actorid aactor);

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
		int				getvalue(EPH aeph)const;

		// # 获取协议字节数
		int				get_bytes()const;

		// # 获取包头长度
		static int		size();

		// # 包头是否接收完毕
		EPH_HEAD_VAL	isready()const;

		// # 对比版本号
		EPH_HEAD_VAL	isversion()const;

		// # 获取协议号
		i32_protocolnum protocolnum()const;

		// # 获取/设置协议号
		i32_protocolnum get_protocolnumber()const;
		void			set_protocol(i32_protocolnum aprotocolnum);

		// # 获取/设置协议类型
		EPROTOCOL_TYPE	get_protocoltype()const;
		void			set_protocoltype(EPROTOCOL_TYPE atype);
		
		EPH_HEAD_VAL push(const char*& abuff, int& alen);
		bool push(ngl::serialize& aflow);
		void reservebuff(char* abuff, int abufflen, std::pair<char*, int>& apair);
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
		std::format_to(out, "HEAD[");
		for (int i = 0; i < ngl::EPH_BYTES; ++i)
		{
			std::format_to(out, "{}#,", val.m_data[i]);
		}
		std::format_to(out, "]");
		return out;
	}
};