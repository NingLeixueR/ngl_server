#pragma once

#include <cstdint>
#include <map>

#include "type.h"
#include "serialize.h"
#include "logserialize.h"
#include "csvtable.h"

namespace ngl
{
	struct pack_head
	{
		int32_t m_data[EPH_SUM] = {0};
		int32_t m_wpos = 0;

		static int32_t m_version;

		pack_head();

		static void		head_set_time(int32_t* abuff);
		static i32_time head_get_time(const int32_t* abuff);
		void			set_time();
		i32_time		get_time()const;

		static void		head_set_version(int32_t* abuff);
		static int32_t  head_get_version(const int32_t* abuff);
		void			set_version();
		int32_t			get_version()const;
		///////////////////////////////////协议号相关///////////////////////////////////////////////
		// 设置actor ################# 发送给哪个actor #### 哪个actor发送的
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

		

		i64_actorid		get_actor()const;
		i64_actorid		get_request_actor()const;
		i16_actortype	get_actortype()const;
		i16_actortype	get_request_actortype()const;
		i16_area		get_actorearea()const;
		i32_actordataid get_actordataid()const;
		void			reset();
		int				getvalue(EPH aeph)const;
		int				get_bytes()const;
		static int		size();
		EPH_HEAD_VAL	isready()const;
		EPH_HEAD_VAL	isversion()const;
		i32_protocolnum protocolnum()const;
		///// 获取协议号
		i32_protocolnum get_protocolnumber()const;
		void			set_protocol(i32_protocolnum aprotocolnum);
		// 设置协议类型
		EPROTOCOL_TYPE	get_protocoltype()const;
		void			set_protocoltype(EPROTOCOL_TYPE atype);
		
		EPH_HEAD_VAL push(const char*& abuff, int& alen);
		bool push(ngl::serialize& aflow);
		void reservebuff(char* abuff, int abufflen, std::pair<char*, int>& apair);
		bool _log(logserialize& atstr, int anumber = EPH_BYTES)const;
		bool log(logserialize& atstr)const;
	};
}
