#pragma once

#include "actor_enum.h"
#include "type.h"
#include "actor_define.h"

namespace ngl
{
	// ############ 64 bit ######### //
	// #actor_type###areaid###id#### //
	// #16bit########16bit####32bit# //
	union actor_guid
	{
		int64_t m_id;
		int32_t m_value1[2];// m_value1[0] = type+area  m_value1[1]=actordataid
		int16_t m_value2[4];// m_value2[0] = type  m_value2[1] = area

		actor_guid();
		actor_guid(int64_t aid);
		actor_guid(ENUM_ACTOR atype, i16_area aareaid, i32_actordataid aid);

		operator i64_actorid ()const;
		i64_actorid id()const;
		ENUM_ACTOR type()const;
		i32_actordataid actordataid()const;
		i16_area area()const;

		static const char* name(i64_actorid aactorid);

		static i64_actorid make(ENUM_ACTOR atype, i16_area aareaid, i32_actordataid aid);
		// none_area()
		static i64_actorid make(ENUM_ACTOR atype, i32_actordataid aid);
		// none_area() none_actordataid()
		static i64_actorid make(ENUM_ACTOR atype);
		// ttab_servers::tab()->m_area none_actordataid() 
		static i64_actorid make_self(ENUM_ACTOR atype);
		// none_type() none_area() none_actordataid()
		static i64_actorid make();
		static i64_actorid make_nonearea(ENUM_ACTOR atype, i32_actordataid aid);
		// ### 替换type
		static i64_actorid make_type(i64_actorid aactorid, ENUM_ACTOR atype);
		// ### 替换area
		static i64_actorid make_area(i64_actorid aactorid, i16_area aareaid);
		// ### 替换actordataid
		static i64_actorid make_actordataid(i64_actorid aactorid, i32_actordataid aid);

		i64_actorid make_type(ENUM_ACTOR atype);
		i64_actorid make_area(i16_area aareaid);
		i64_actorid make_actordataid(i32_actordataid aid);

		// ### 获取actordataid
		static i32_actordataid actordataid(i64_actorid aactorid);
		// ### 获取type
		static i16_actortype type(i64_actorid aactorid);		
		// ### 获取area
		static i16_area area(i64_actorid aactorid);

		// sendmore 发送给同类型的所有actor
		static i64_actorid moreactor(ENUM_ACTOR atype);
		static i64_actorid moreactor();
		void none();
		//是否发给同类型的所有actor
		static bool is_moreactor(i64_actorid actorid, ENUM_ACTOR atype);
		bool is_moreactor(ENUM_ACTOR atype)const;
		//actor type 是否无效
		static bool is_actortypenone(i64_actorid actorid);
		bool is_actortypenone()const;
		//actor area 是否无效
		static bool is_actorareanone(i64_actorid actorid);
		bool is_actorareanone()const;
		//actor id 是否无效
		static bool is_actoridnone(i64_actorid actorid);
		bool is_actoridnone()const;

		static bool is_single(ENUM_ACTOR atype);

		static i16_area none_area();
		static ENUM_ACTOR none_type();
		static i32_actordataid none_actordataid();

		template <typename T>
		static T none()
		{
			return (T)(-1);
		}

		bool operator<(int64_t ar)const;
		bool operator<(const actor_guid& r)const;
		bool operator==(int64_t ar)const;
		bool operator==(const actor_guid& r)const;
		bool operator!=(int64_t ar)const;
		bool operator!=(const actor_guid& r)const;
		bool operator>(int64_t ar)const;
		bool operator>(const actor_guid& r)const;

		//def_portocol_function(actor_guid, m_id)
		def_portocol_function(actor_guid, m_id)

	};
}


#define actor_guid_none_area actor_guid::none<i16_area>()