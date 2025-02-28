#pragma once

#include "serialize.h"
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

		// # ����mask
		static void				head_set_mask(int32_t* abuff);
		static EPH_HEAD_VAL		head_check_mask(const int32_t* abuff, int awpos);
		void					set_mask();
		EPH_HEAD_VAL			check_mask()const;

		// # ����ͷ���û��߻�ȡʱ��
		static void		head_set_time(int32_t* abuff);
		static i32_time head_get_time(const int32_t* abuff);
		void			set_time();
		i32_time		get_time()const;

		// # ����actor 
		// # aactor:���͸��ĸ�actor /* aenum == ACTOR_NONE ��ֵ��Ч */
		// # arequestactorid:�ĸ�actor���͵�
		static void head_set_actor(int32_t* abuff, i64_actorid aactor, i64_actorid arequestactorid);
		void set_actor(i64_actorid aactor, i64_actorid arequestactorid);
		void set_requestactor(i64_actorid arequestactorid);
		static void set_actor(int32_t* abuff, i64_actorid aactor);

		// # ��ȡҪ���͸��ĸ�actor
		i64_actorid		get_actor()const;

		// # ��ȡ˭���͵�actor
		i64_actorid		get_request_actor()const;

		// # ��ȡҪ���͸���actor��type
		i16_actortype	get_actortype()const;

		// # ��ȡ˭���͵�actor��type
		i16_actortype	get_request_actortype()const;

		// # ��ȡҪ���͸���actor��area
		i16_area		get_actorarea()const;

		// # ��ȡҪ���͸���actor��dataid
		i32_actordataid get_actordataid()const;

		// # ����head
		void			reset();

		// # ��ȡEPH��Ӧ��ֵ
		int32_t			getvalue(EPH aeph)const;

		// # ��ȡЭ���ֽ���
		int32_t			get_bytes()const;

		// # ��ȡ��ͷ����
		static int32_t	size();

		// # ��ͷ�Ƿ�������
		EPH_HEAD_VAL	isready()const;

		// # ��ȡЭ���
		i32_protocolnum protocolnum()const;

		// # ��ȡ/����Э���
		i32_protocolnum get_protocolnumber()const;
		void			set_protocol(i32_protocolnum aprotocolnum);

		// # ��ȡ/����Э������
		EPROTOCOL_TYPE	get_protocoltype()const;
		void			set_protocoltype(EPROTOCOL_TYPE atype);
		
		EPH_HEAD_VAL push(const char*& abuff, int32_t& alen);
		bool push(ngl::serialize& aflow);
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
		std::format_to(out, "HEAD[MASK:{},BYTES:{},TIME:{},PROTOCOLNUM:{},PROTOCOLTYPE:{},ACTOR:{},REQUEST_ACTOR:{}]", 
			ss, 
			val.m_data[ngl::EPH_BYTES],
			ngl::localtime::time2str(val.m_data[ngl::EPH_TIME], "%y/%m/%d %H:%M:%S"),
			val.m_data[ngl::EPH_PROTOCOLNUM],
			val.m_data[ngl::EPH_PROTOCOLTYPE],
			lactor,
			lrequestactor
		);
		return out;
	}
};