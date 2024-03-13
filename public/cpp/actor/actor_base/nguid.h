#pragma once

#include "nactortype.h"
#include "type.h"
#include "ndefine.h"

namespace ngl
{
	// actor唯一id(64bit)
	// 16位			类型ENUM_ACTOR
	// 16位			区服id	
	// 32位			数据id
	// ############ 64 bit ######### //
	// #actor_type###areaid###id#### //
	// #16bit########16bit####32bit# //
	union nguid
	{
		int64_t m_id;
		int32_t m_value1[2];// m_value1[0] = type+area  m_value1[1]=actordataid
		int16_t m_value2[4];// m_value2[0] = type  m_value2[1] = area

		nguid();
		nguid(int64_t aid);
		nguid(ENUM_ACTOR atype, i16_area aareaid, i32_actordataid aid);

		//# 可以将union强转为i64_actorid值
		operator i64_actorid ()const;
		//# 获取guidid值
		i64_actorid id()const;
		//# 获取ENUM_ACTOR类型
		ENUM_ACTOR type()const;
		//# 获取数据id
		i32_actordataid actordataid()const;
		//# 获取区服
		i16_area area()const;
		//# 通过nactortype::enum2name获取枚举对应字符串
		static const char* name(i64_actorid aactorid);

		//# 根据类型、区服、数据id创建一个i64_actorid
		static i64_actorid make(ENUM_ACTOR atype, i16_area aareaid, i32_actordataid aid);
		//# 根据类型、数据id、区服为none_area()创建一个i64_actorid
		static i64_actorid make(ENUM_ACTOR atype, i32_actordataid aid);
		//# 根据类型、数据id为none_actordataid()、区服为none_area()创建一个i64_actorid
		static i64_actorid make(ENUM_ACTOR atype);
		//# 根据类型none_type()、区服为none_area()数据id为none_actordataid()创建一个i64_actorid
		static i64_actorid make();
		//# 根据类型、区服为ttab_servers::tab()->m_area none_actordataid()、数据id为none_actordataid创建一个i64_actorid
		static i64_actorid make_self(ENUM_ACTOR atype);
		//# 替换type
		static i64_actorid make_type(i64_actorid aactorid, ENUM_ACTOR atype);
		//# 替换area
		static i64_actorid make_area(i64_actorid aactorid, i16_area aareaid);
		//# 替换actordataid
		static i64_actorid make_actordataid(i64_actorid aactorid, i32_actordataid aid);
		//# 替换type
		i64_actorid make_type(ENUM_ACTOR atype);
		//# 替换area
		i64_actorid make_area(i16_area aareaid);
		//# 替换actordataid
		i64_actorid make_actordataid(i32_actordataid aid);

		//# 获取actordataid
		static i32_actordataid actordataid(i64_actorid aactorid);
		// ### 获取type
		static i16_actortype type(i64_actorid aactorid);		
		// ### 获取area
		static i16_area area(i64_actorid aactorid);

		// sendmore 发送给同类型的所有actor
		static i64_actorid moreactor(ENUM_ACTOR atype);
		// 和无参make()一致
		static i64_actorid moreactor();
		// 和无参make()一致
		void none();

		// 是否发给同类型的所有actor
		static bool is_moreactor(i64_actorid actorid, ENUM_ACTOR atype);
		bool is_moreactor(ENUM_ACTOR atype)const;

		// actor type 是否无效
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
		bool operator<(const nguid& r)const;
		bool operator==(int64_t ar)const;
		bool operator==(const nguid& r)const;
		bool operator!=(int64_t ar)const;
		bool operator!=(const nguid& r)const;
		bool operator>(int64_t ar)const;
		bool operator>(const nguid& r)const;

		def_portocol_function(nguid, m_id)
	};
}//namespace ngl

#define nguid_none_area nguid::none<i16_area>()