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

		//# 获取type
		static i16_actortype type(i64_actorid aactorid);

		//# 获取area
		static i16_area area(i64_actorid aactorid);

		//# sendmore 发送给同类型的所有actor
		static i64_actorid moreactor(ENUM_ACTOR atype);

		//# 和无参make()一致
		static i64_actorid moreactor();

		//# 和无参make()一致
		void none();

		//# 是否发给同类型的所有actor
		static bool is_moreactor(i64_actorid actorid, ENUM_ACTOR atype);

		bool is_moreactor(ENUM_ACTOR atype)const;

		//# actor type 是否无效
		static bool is_actortypenone(i64_actorid actorid);

		bool is_actortypenone()const;

		//# actor area 是否无效
		static bool is_actorareanone(i64_actorid actorid);

		//# actor area 是否无效
		bool is_actorareanone()const;

		//# actor id 是否无效
		static bool is_actoridnone(i64_actorid actorid);

		//# actor id 是否无效
		bool is_actoridnone()const;

		//# ENUM_ACTOR 是否单例
		static bool is_single(ENUM_ACTOR atype);

		//# 无效area
		static i16_area none_area();

		//# 无效type
		static ENUM_ACTOR none_type();

		//# 无效dataid
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

		nlogactor(int32_t avalue) :
			m_value32(avalue)
		{}

		nlogactor(ENUM_ACTOR avalue1, ELOG_TYPE avalue2) :
			m_value32(0)
		{
			m_value16[0] = avalue1;
			m_value16[1] = avalue2;
		}

		ENUM_ACTOR actor_type()
		{
			return (ENUM_ACTOR)m_value16[0];
		}

		ELOG_TYPE log_type()
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