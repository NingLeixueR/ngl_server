#pragma once

#include "nactortype.h"
#include "enum2name.h"
#include "ndefine.h"
#include "type.h"

#include <format>

namespace ngl
{
	///////////////////////////////////
	// actorΨһid(64bit)			 //
	// 16λ			����ENUM_ACTOR	 //
	// 16λ			����id			 //
	// 32λ			����id			 //
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

		//# ���Խ�unionǿתΪi64_actoridֵ
		inline operator i64_actorid ()const
		{
			return m_id;
		}

		//# ��ȡguididֵ
		inline i64_actorid id()const
		{
			return m_id;
		}

		//# ��ȡENUM_ACTOR����
		inline ENUM_ACTOR type()const
		{
			return (ENUM_ACTOR)m_value2[0];
		}

		//# ��ȡ����id
		inline i32_actordataid actordataid()const
		{
			return m_value1[1];
		}

		//# ��ȡ����
		inline i16_area area()const
		{
			return m_value2[1];
		}

		//# ͨ��nactortype::enum2name��ȡö�ٶ�Ӧ�ַ���
		static const char* name(i64_actorid aactorid)
		{
			nguid lguid(aactorid);
			i16_actortype ltype = lguid.type();
			return em<ENUM_ACTOR>::get_name((ENUM_ACTOR)ltype);
		}

		//# �������͡�����������id����һ��i64_actorid
		static i64_actorid make(ENUM_ACTOR atype, i16_area aareaid, i32_actordataid aid)
		{
			nguid lguid(atype, aareaid, aid);
			return (i64_actorid)lguid;
		}

		//# �������͡�����id������Ϊnone_area()����һ��i64_actorid
		static i64_actorid make(ENUM_ACTOR atype, i32_actordataid aid)
		{
			nguid lguid(atype, none_area(), aid);
			return (i64_actorid)lguid;
		}

		//# �������͡�����idΪnone_actordataid()������Ϊnone_area()����һ��i64_actorid
		static i64_actorid make(ENUM_ACTOR atype)
		{
			nguid lguid(atype, none_area(), none_actordataid());
			return (i64_actorid)lguid;
		}

		//# ��������none_type()������Ϊnone_area()����idΪnone_actordataid()����һ��i64_actorid
		static i64_actorid make()
		{
			nguid lguid(none_type(), none_area(), none_actordataid());
			return (i64_actorid)lguid;
		}

		//# �������͡�����Ϊtab_self_area none_actordataid()������idΪnone_actordataid����һ��i64_actorid
		static i64_actorid make_self(ENUM_ACTOR atype);

		//# �滻type
		static i64_actorid make_type(i64_actorid aactorid, ENUM_ACTOR atype)
		{
			nguid lguid(aactorid);
			return lguid.make_type(atype);
		}

		//# �滻area
		static i64_actorid make_area(i64_actorid aactorid, i16_area aareaid)
		{
			nguid lguid(aactorid);
			return lguid.make_area(aareaid);
		}

		//# �滻actordataid
		static i64_actorid make_actordataid(i64_actorid aactorid, i32_actordataid aid)
		{
			nguid lguid(aactorid);
			return lguid.make_actordataid(aid);
		}

		//# �滻type
		inline i64_actorid make_type(ENUM_ACTOR atype)
		{
			m_value2[0] = atype;
			return (i64_actorid)(*this);
		}

		//# �滻area
		inline i64_actorid make_area(i16_area aareaid)
		{
			m_value2[1] = aareaid;
			return (i64_actorid)(*this);
		}

		//# �滻actordataid
		inline i64_actorid make_actordataid(i32_actordataid aid)
		{
			m_value2[0] = aid;
			return (i64_actorid)(*this);
		}

		//# ��ȡactordataid
		static i32_actordataid actordataid(i64_actorid aactorid)
		{
			nguid lguid(aactorid);
			return lguid.actordataid();
		}

		//# ��ȡtype
		static i16_actortype type(i64_actorid aactorid)
		{
			nguid lguid(aactorid);
			return lguid.type();
		}

		//# ��ȡarea
		static i16_area area(i64_actorid aactorid)
		{
			nguid lguid(aactorid);
			return lguid.area();
		}

		//# sendmore ���͸�ͬ���͵�����actor
		static i64_actorid moreactor(ENUM_ACTOR atype)
		{
			return make(atype, none_area(), none_actordataid());
		}

		//# ���޲�make()һ��
		static i64_actorid moreactor()
		{
			return make(none_type(), none_area(), none_actordataid());
		}

		//# ���޲�make()һ��
		inline void none()
		{
			m_value2[0] = none_type();
			m_value2[1] = none_area();
			m_value1[1] = none_actordataid();
		}

		//# �Ƿ񷢸�ͬ���͵�����actor
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

		//# actor type �Ƿ���Ч
		static bool is_actortypenone(i64_actorid actorid)
		{
			nguid lguid(actorid);
			return lguid.is_actortypenone();
		}

		inline bool is_actortypenone()const
		{
			return type() == none<i16_actortype>();
		}

		//# actor area �Ƿ���Ч
		static bool is_actorareanone(i64_actorid actorid)
		{
			nguid lguid(actorid);
			return lguid.is_actorareanone();
		}

		//# actor area �Ƿ���Ч
		inline bool is_actorareanone()const
		{
			return area() == none<i16_area>();
		}

		//# actor id �Ƿ���Ч
		static bool is_actoridnone(i64_actorid actorid)
		{
			nguid lguid(actorid);
			return lguid.is_actoridnone();
		}

		//# actor id �Ƿ���Ч
		inline bool is_actoridnone()const
		{
			return actordataid() == none<i32_actordataid>();
		}

		//# ��Чarea
		static i16_area none_area()
		{
			return none<i16_area>();
		}

		//# ��Чtype
		static ENUM_ACTOR none_type()
		{
			return none<ENUM_ACTOR>();
		}

		//# ��Чdataid
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
	// ��������actor_log��guid��32λdataid����		   //
	// 16λ���ڴ洢��¼��־��actor��ENUM_ACTOR		   //
	// 16λ���ڴ洢��־����(������־��������־��bi��־)//
	/////////////////////////////////////////////////////
	union nlogactor
	{
		nlogactor() = delete;

		int32_t m_value32;
		int16_t m_value16[2];	//ENUM_ACTOR aactortype, ELOG_TYPE alogtype

		static int16_t m_localnum;

		explicit nlogactor(int32_t avalue) :
			m_value32(avalue)
		{}

		nlogactor(ENUM_ACTOR avalue1, ELOG_TYPE avalue2);

		inline ENUM_ACTOR actor_type()const
		{
			return (ENUM_ACTOR)m_value16[0];
		}

		inline ELOG_TYPE log_type()const
		{
			if ((0x8000 & m_value16[1]) != 0)
			{
				return ELOG_TYPE::ELOG_LOCAL;
			}
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

}