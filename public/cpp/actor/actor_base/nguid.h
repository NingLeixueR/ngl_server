/*
* Copyright (c) [2020-2025] NingLeixueR
* 
* 项目名称：ngl_server
* 项目地址：https://github.com/NingLeixueR/ngl_server
* 
* 本文件是 ngl_server 项目的一部分，遵循 MIT 开源协议发布。
* 您可以按照协议规定自由使用、修改和分发本项目，包括商业用途，
* 但需保留原始版权和许可声明。
* 
* 许可详情参见项目根目录下的 LICENSE 文件：
* https://github.com/NingLeixueR/ngl_server/blob/main/LICENSE
*/
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

		//# 根据类型、区服为tab_self_area none_actordataid()、数据id为none_actordataid创建一个i64_actorid
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

		//# send 发送给同类型的所有actor
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

		dprotocol(nguid, m_id)
	};
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
		const char* lanme = ngl::em<ngl::ENUM_ACTOR>::get_name(aval.type());
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

namespace ngl
{
	template <>
	struct lexical_cast2<nguid>
	{
		static nguid fun(const std::string& source)
		{
			int64_t lvalue = tools::lexical_cast<int64_t>(source);
			return nguid(lvalue);
		}

		static nguid fun(const char* source)
		{
			int64_t lvalue = tools::lexical_cast<int64_t>(source);
			return nguid(lvalue);
		}
	};

	template <>
	struct lexical_cast2<std::string>
	{
		template <typename Source>
		static std::string fun(const Source& source)
		{
			return std::format("{}", source);
		}

		static std::string fun(const std::string& source)
		{
			return source;
		}

		static std::string fun(const char* source)
		{
			return source;
		}

		static std::string fun(const nguid& source)
		{
			return tools::lexical_cast<std::string>((int64_t)(source));
		}
	};

	template <typename To, typename From>
	To tools::lexical_cast(const From& from)
	{
		return lexical_cast2<To>::fun(from);
	}

	template <typename To>
	To& tools::lexical_cast(To& from)
	{
		return from;
	}
}//namespace ngl