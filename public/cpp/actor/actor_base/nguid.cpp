#include "ttab_servers.h"
#include "nguid.h"

namespace ngl
{
	nguid::nguid()
	{
		m_value2[0] = none_type();
		m_value2[1] = none_area();
		m_value1[1] = none_actordataid();
	}

	nguid::nguid(int64_t aid) :
		m_id(aid)
	{}

	nguid::nguid(ENUM_ACTOR atype, i16_area aareaid, i32_actordataid aid) :
		m_id(0)
	{
		m_value2[0] = atype;
		m_value2[1] = aareaid;
		m_value1[1] = aid;
	}

	nguid::operator i64_actorid ()const
	{
		return m_id;
	}

	i64_actorid nguid::id()const
	{
		return m_id;
	}

	const char* nguid::name(i64_actorid aactorid)
	{
		nguid lguid(aactorid);
		i16_actortype ltype = lguid.type();
		return em<ENUM_ACTOR>::get_name((ENUM_ACTOR)ltype);
	}

	i64_actorid nguid::make(ENUM_ACTOR atype, i16_area aareaid, i32_actordataid aid)
	{
		nguid lguid(atype, aareaid, aid);
		return (i64_actorid)lguid;
	}

	i64_actorid nguid::make(ENUM_ACTOR atype, i32_actordataid aid)
	{
		nguid lguid(atype, none_area(), aid);
		return (i64_actorid)lguid;
	}

	i64_actorid nguid::make(ENUM_ACTOR atype)
	{
		nguid lguid(atype, none_area(), none_actordataid());
		return (i64_actorid)lguid;
	}

	i64_actorid nguid::make_self(ENUM_ACTOR atype)
	{
		nguid lguid(atype, ttab_servers::tab()->m_area, none_actordataid());
		return (i64_actorid)lguid;
	}

	i64_actorid nguid::make()
	{
		nguid lguid(none_type(), none_area(), none_actordataid());
		return (i64_actorid)lguid;
	}

	i64_actorid nguid::make_type(i64_actorid aactorid, ENUM_ACTOR atype)
	{
		nguid lguid(aactorid);
		return lguid.make_type(atype);
	}

	i64_actorid nguid::make_type(ENUM_ACTOR atype)
	{
		m_value2[0] = atype;
		return (i64_actorid)(*this);
	}

	i64_actorid nguid::make_area(i64_actorid aactorid, i16_area aareaid)
	{
		nguid lguid(aactorid);
		return lguid.make_area(aareaid);
	}

	i64_actorid nguid::make_area(i16_area aareaid)
	{
		m_value2[1] = aareaid;
		return (i64_actorid)(*this);
	}

	i64_actorid nguid::make_actordataid(i64_actorid aactorid, i32_actordataid aid)
	{
		nguid lguid(aactorid);
		return lguid.make_actordataid(aid);
	}

	i64_actorid nguid::make_actordataid(i32_actordataid aid)
	{
		m_value2[0] = aid;
		return (i64_actorid)(*this);
	}

	i32_actordataid nguid::actordataid(i64_actorid aactorid)
	{
		nguid lguid(aactorid);
		return lguid.actordataid();
	}

	i32_actordataid nguid::actordataid()const
	{
		return m_value1[1];
	}

	i16_actortype nguid::type(i64_actorid aactorid)
	{
		nguid lguid(aactorid);
		return lguid.type();
	}

	ENUM_ACTOR nguid::type()const
	{
		return (ENUM_ACTOR)m_value2[0];
	}

	i16_area nguid::area(i64_actorid aactorid)
	{
		nguid lguid(aactorid);
		return lguid.area();
	}

	i16_area nguid::area()const
	{
		return m_value2[1];
	}

	i64_actorid nguid::moreactor(ENUM_ACTOR atype)
	{
		return make(atype, none_area(), none_actordataid());
	}

	i64_actorid nguid::moreactor()
	{
		return make(none_type(), none_area(), none_actordataid());
	}

	void nguid::none()
	{
		m_value2[0] = none_type();
		m_value2[1] = none_area();
		m_value1[1] = none_actordataid();
	}

	bool nguid::is_moreactor(i64_actorid actorid, ENUM_ACTOR atype)
	{
		nguid lguid(actorid);
		return lguid.is_moreactor(atype);
	}

	bool nguid::is_moreactor(ENUM_ACTOR atype)const
	{
		nguid lguid(atype, none_area(), none_actordataid());
		return (i64_actorid)(lguid) == (i64_actorid)(*this);
	}

	bool nguid::is_actortypenone(i64_actorid actorid)
	{
		nguid lguid(actorid);
		return lguid.is_actortypenone();
	}

	bool nguid::is_actortypenone()const
	{
		return type() == none<i16_actortype>();
	}

	bool nguid::is_actorareanone(i64_actorid actorid)
	{
		nguid lguid(actorid);
		return lguid.is_actorareanone();
	}

	bool nguid::is_actorareanone()const
	{
		return area() == none<i16_area>();
	}

	bool nguid::is_actoridnone(i64_actorid actorid)
	{
		nguid lguid(actorid);
		return lguid.is_actoridnone();
	}

	bool nguid::is_actoridnone()const
	{
		return actordataid() == none<i32_actordataid>();
	}

	bool nguid::is_single(ENUM_ACTOR atype)
	{
		return atype >= ACTOR_SIGNLE_START && atype < ACTOR_SIGNLE_FINISH;
	}

	i16_area nguid::none_area()
	{
		return none<i16_area>();
	}

	ENUM_ACTOR nguid::none_type()
	{
		return none<ENUM_ACTOR>();
	}

	i32_actordataid nguid::none_actordataid()
	{
		return none<i32_actordataid>();
	}

	bool nguid::operator<(int64_t ar)const
	{
		return m_id < ar;
	}

	bool nguid::operator<(const nguid& r)const
	{
		return m_id < r.m_id;
	}

	bool nguid::operator==(int64_t ar)const
	{
		return m_id == ar;
	}

	bool nguid::operator==(const nguid& r)const
	{
		return m_id == r.m_id;
	}

	bool nguid::operator!=(int64_t ar)const
	{
		return m_id != ar;
	}

	bool nguid::operator!=(const nguid& r)const
	{
		return m_id != r.m_id;
	}

	bool nguid::operator>(int64_t ar)const
	{
		return m_id > ar;
	}

	bool nguid::operator>(const nguid& r)const
	{
		return m_id > r.m_id;
	}
}//namespace ngl