#pragma once

#include "nactortype.h"
#include "enum2name.h"
#include "ndefine.h"
#include "type.h"

#include <format>

namespace ngl
{
	///////////////////////////////////
	// actor唯一id(64bit)			 //
	// 16位			类型ENUM_ACTOR	 //
	// 16位			区服id			 //
	// 32位			数据id			 //
	// ############ 64 bit ######### //
	// #actor_type###areaid###id#### //
	// #16bit########16bit####32bit# //
	///////////////////////////////////
	union nguid
	{
	private:
		int64_t m_id;
		int32_t m_value1[2];// m_value1[0] = type+area  m_value1[1]=actordataid
		int16_t m_value2[4];// m_value2[0] = type		m_value2[1] = area
	public:
		inline nguid()
		{
			m_value2[0] = none_type();
			m_value2[1] = none_area();
			m_value1[1] = none_actordataid();
		}

		inline nguid(int64_t aid) :
			m_id(aid)
		{}

		inline nguid(ENUM_ACTOR atype, i16_area aareaid, i32_actordataid aid) :
			m_id(0)
		{
			m_value2[0] = atype;
			m_value2[1] = aareaid;
			m_value1[1] = aid;
		}

		//# 可以将union强转为i64_actorid值
		inline operator i64_actorid ()const
		{
			return m_id;
		}

		//# 获取guidid值
		inline i64_actorid id()const
		{
			return m_id;
		}

		//# 获取ENUM_ACTOR类型
		inline ENUM_ACTOR type()const
		{
			return (ENUM_ACTOR)m_value2[0];
		}

		//# 获取数据id
		inline i32_actordataid actordataid()const
		{
			return m_value1[1];
		}

		//# 获取区服
		inline i16_area area()const
		{
			return m_value2[1];
		}

		//# 通过nactortype::enum2name获取枚举对应字符串
		static const char* name(i64_actorid aactorid)
		{
			nguid lguid(aactorid);
			i16_actortype ltype = lguid.type();
			return em<ENUM_ACTOR>::get_name((ENUM_ACTOR)ltype);
		}

		//# 根据类型、区服、数据id创建一个i64_actorid
		static i64_actorid make(ENUM_ACTOR atype, i16_area aareaid, i32_actordataid aid)
		{
			nguid lguid(atype, aareaid, aid);
			return (i64_actorid)lguid;
		}

		//# 根据类型、数据id、区服为none_area()创建一个i64_actorid
		static i64_actorid make(ENUM_ACTOR atype, i32_actordataid aid)
		{
			nguid lguid(atype, none_area(), aid);
			return (i64_actorid)lguid;
		}

		//# 根据类型、数据id为none_actordataid()、区服为none_area()创建一个i64_actorid
		static i64_actorid make(ENUM_ACTOR atype)
		{
			nguid lguid(atype, none_area(), none_actordataid());
			return (i64_actorid)lguid;
		}

		//# 根据类型none_type()、区服为none_area()数据id为none_actordataid()创建一个i64_actorid
		static i64_actorid make()
		{
			nguid lguid(none_type(), none_area(), none_actordataid());
			return (i64_actorid)lguid;
		}

		//# 根据类型、区服为ttab_servers::tab()->m_area none_actordataid()、数据id为none_actordataid创建一个i64_actorid
		static i64_actorid make_self(ENUM_ACTOR atype);

		//# 替换type
		static i64_actorid make_type(i64_actorid aactorid, ENUM_ACTOR atype)
		{
			nguid lguid(aactorid);
			return lguid.make_type(atype);
		}

		//# 替换area
		static i64_actorid make_area(i64_actorid aactorid, i16_area aareaid)
		{
			nguid lguid(aactorid);
			return lguid.make_area(aareaid);
		}

		//# 替换actordataid
		static i64_actorid make_actordataid(i64_actorid aactorid, i32_actordataid aid)
		{
			nguid lguid(aactorid);
			return lguid.make_actordataid(aid);
		}

		//# 替换type
		inline i64_actorid make_type(ENUM_ACTOR atype)
		{
			m_value2[0] = atype;
			return (i64_actorid)(*this);
		}

		//# 替换area
		inline i64_actorid make_area(i16_area aareaid)
		{
			m_value2[1] = aareaid;
			return (i64_actorid)(*this);
		}

		//# 替换actordataid
		inline i64_actorid make_actordataid(i32_actordataid aid)
		{
			m_value2[0] = aid;
			return (i64_actorid)(*this);
		}

		//# 获取actordataid
		static i32_actordataid actordataid(i64_actorid aactorid)
		{
			nguid lguid(aactorid);
			return lguid.actordataid();
		}

		//# 获取type
		static i16_actortype type(i64_actorid aactorid)
		{
			nguid lguid(aactorid);
			return lguid.type();
		}

		//# 获取area
		static i16_area area(i64_actorid aactorid)
		{
			nguid lguid(aactorid);
			return lguid.area();
		}

		//# sendmore 发送给同类型的所有actor
		static i64_actorid moreactor(ENUM_ACTOR atype)
		{
			return make(atype, none_area(), none_actordataid());
		}

		//# 和无参make()一致
		static i64_actorid moreactor()
		{
			return make(none_type(), none_area(), none_actordataid());
		}

		//# 和无参make()一致
		inline void none()
		{
			m_value2[0] = none_type();
			m_value2[1] = none_area();
			m_value1[1] = none_actordataid();
		}

		//# 是否发给同类型的所有actor
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

		//# actor type 是否无效
		static bool is_actortypenone(i64_actorid actorid)
		{
			nguid lguid(actorid);
			return lguid.is_actortypenone();
		}

		inline bool is_actortypenone()const
		{
			return type() == none<i16_actortype>();
		}

		//# actor area 是否无效
		static bool is_actorareanone(i64_actorid actorid)
		{
			nguid lguid(actorid);
			return lguid.is_actorareanone();
		}

		//# actor area 是否无效
		inline bool is_actorareanone()const
		{
			return area() == none<i16_area>();
		}

		//# actor id 是否无效
		static bool is_actoridnone(i64_actorid actorid)
		{
			nguid lguid(actorid);
			return lguid.is_actoridnone();
		}

		//# actor id 是否无效
		inline bool is_actoridnone()const
		{
			return actordataid() == none<i32_actordataid>();
		}

		//# 无效area
		static i16_area none_area()
		{
			return none<i16_area>();
		}

		//# 无效type
		static ENUM_ACTOR none_type()
		{
			return none<ENUM_ACTOR>();
		}

		//# 无效dataid
		static i32_actordataid none_actordataid()
		{
			return none<i32_actordataid>();
		}

		template <typename T>
		static T none()
		{
			return (T)(-1);
		}

		inline bool operator<(int64_t ar)const
		{
			return m_id < ar;
		}

		inline bool operator<(const nguid& r)const
		{
			return m_id < r.m_id;
		}

		inline bool operator==(int64_t ar)const
		{
			return m_id == ar;
		}

		inline bool operator==(const nguid& r)const
		{
			return m_id == r.m_id;
		}

		inline bool operator!=(int64_t ar)const
		{
			return m_id != ar;
		}

		inline bool operator!=(const nguid& r)const
		{
			return m_id != r.m_id;
		}

		inline bool operator>(int64_t ar)const
		{
			return m_id > ar;
		}

		inline bool operator>(const nguid& r)const
		{
			return m_id > r.m_id;
		}

		def_portocol_function(nguid, m_id)
	};

	/////////////////////////////////////////////////////
	// 用于生成actor_log的guid的32位dataid部分		   //
	// 16位用于存储记录日志的actor的ENUM_ACTOR		   //
	// 16位用于存储日志类型(本地日志、网络日志、bi日志)//
	/////////////////////////////////////////////////////
	union nlogactor
	{
		nlogactor() = delete;

		int32_t m_value32;
		int16_t m_value16[2];	//ENUM_ACTOR aactortype, ELOG_TYPE alogtype

		inline nlogactor(int32_t avalue) :
			m_value32(avalue)
		{}

		inline nlogactor(ENUM_ACTOR avalue1, ELOG_TYPE avalue2) :
			m_value32(0)
		{
			m_value16[0] = avalue1;
			m_value16[1] = avalue2;
		}

		inline ENUM_ACTOR actor_type()
		{
			return (ENUM_ACTOR)m_value16[0];
		}

		inline ELOG_TYPE log_type()
		{
			return (ELOG_TYPE)m_value16[1];
		}

		static ENUM_ACTOR actor_type(int32_t avalue)
		{
			nlogactor ltemp(avalue);
			return ltemp.actor_type();
		}

		static ELOG_TYPE log_type(int32_t avalue)
		{
			nlogactor ltemp(avalue);
			return ltemp.log_type();
		}
	};
}//namespace ngl

template <>
struct std::formatter<ngl::nguid>
{
	constexpr auto parse(std::format_parse_context& ctx)
	{
		return ctx.begin();
	}

	auto format(const ngl::nguid& aval, std::format_context& ctx)const
	{
		const char* lanme = ngl::em<ngl::ENUM_ACTOR>::get_name(aval.type());
		if (lanme == nullptr)
		{
			return std::format_to(ctx.out()
				, "guid<{}-{}-{}>"
				, (int)aval.type()
				, aval.area()
				, aval.actordataid()
			);
		}
		else
		{
			return std::format_to(ctx.out()
				, "guid<{}-{}-{}>"
				, lanme
				, aval.area()
				, aval.actordataid()
			);
		}
	}
};