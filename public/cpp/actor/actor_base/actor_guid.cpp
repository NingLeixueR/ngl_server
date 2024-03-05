#include "actor_guid.h"
#include "ttab_servers.h"

namespace ngl
{
	actor_guid::actor_guid()
	{
		m_value2[0] = none_type();
		m_value2[1] = none_area();
		m_value1[1] = none_actordataid();
	}

	actor_guid::actor_guid(int64_t aid) :
		m_id(aid)
	{}

	actor_guid::actor_guid(ENUM_ACTOR atype, i16_area aareaid, i32_actordataid aid) :
		m_id(0)
	{
		m_value2[0] = atype;
		m_value2[1] = aareaid;
		m_value1[1] = aid;
	}

	actor_guid::operator i64_actorid ()const
	{
		return m_id;
	}

	i64_actorid actor_guid::id()const
	{
		return m_id;
	}

	const char* actor_guid::name(i64_actorid aactorid)
	{
		actor_guid lguid(aactorid);
		i16_actortype ltype = lguid.type();
		return actor_typename::enum2name((ENUM_ACTOR)ltype);
	}

	i64_actorid actor_guid::make(ENUM_ACTOR atype, i16_area aareaid, i32_actordataid aid)
	{
		actor_guid lguid(atype, aareaid, aid);
		return (i64_actorid)lguid;
	}

	i64_actorid actor_guid::make(ENUM_ACTOR atype, i32_actordataid aid)
	{
		actor_guid lguid(atype, none_area(), aid);
		return (i64_actorid)lguid;
	}

	i64_actorid actor_guid::make(ENUM_ACTOR atype)
	{
		actor_guid lguid(atype, none_area(), none_actordataid());
		return (i64_actorid)lguid;
	}

	i64_actorid actor_guid::make_self(ENUM_ACTOR atype)
	{
		actor_guid lguid(atype, ttab_servers::tab()->m_area, none_actordataid());
		return (i64_actorid)lguid;
	}

	i64_actorid actor_guid::make()
	{
		actor_guid lguid(none_type(), none_area(), none_actordataid());
		return (i64_actorid)lguid;
	}

	i64_actorid actor_guid::make_type(i64_actorid aactorid, ENUM_ACTOR atype)
	{
		actor_guid lguid(aactorid);
		return lguid.make_type(atype);
	}

	i64_actorid actor_guid::make_type(ENUM_ACTOR atype)
	{
		m_value2[0] = atype;
		return (i64_actorid)(*this);
	}

	i64_actorid actor_guid::make_area(i64_actorid aactorid, i16_area aareaid)
	{
		actor_guid lguid(aactorid);
		return lguid.make_area(aareaid);
	}

	i64_actorid actor_guid::make_area(i16_area aareaid)
	{
		m_value2[1] = aareaid;
		return (i64_actorid)(*this);
	}

	i64_actorid actor_guid::make_actordataid(i64_actorid aactorid, i32_actordataid aid)
	{
		actor_guid lguid(aactorid);
		return lguid.make_actordataid(aid);
	}

	i64_actorid actor_guid::make_actordataid(i32_actordataid aid)
	{
		m_value2[0] = aid;
		return (i64_actorid)(*this);
	}

	i32_actordataid actor_guid::actordataid(i64_actorid aactorid)
	{
		actor_guid lguid(aactorid);
		return lguid.actordataid();
	}

	i32_actordataid actor_guid::actordataid()const
	{
		return m_value1[1];
	}

	i16_actortype actor_guid::type(i64_actorid aactorid)
	{
		actor_guid lguid(aactorid);
		return lguid.type();
	}

	ENUM_ACTOR actor_guid::type()const
	{
		return (ENUM_ACTOR)m_value2[0];
	}

	i16_area actor_guid::area(i64_actorid aactorid)
	{
		actor_guid lguid(aactorid);
		return lguid.area();
	}

	i16_area actor_guid::area()const
	{
		return m_value2[1];
	}

	// sendmore 发送给同类型的所有actor
	i64_actorid actor_guid::moreactor(ENUM_ACTOR atype)
	{
		return make(atype, none_area(), none_actordataid());
	}

	i64_actorid actor_guid::moreactor()
	{
		return make(none_type(), none_area(), none_actordataid());
	}

	void actor_guid::none()
	{
		m_value2[0] = none_type();
		m_value2[1] = none_area();
		m_value1[1] = none_actordataid();
	}

	//是否发给同类型的所有actor
	bool actor_guid::is_moreactor(i64_actorid actorid, ENUM_ACTOR atype)
	{
		actor_guid lguid(actorid);
		return lguid.is_moreactor(atype);
	}

	bool actor_guid::is_moreactor(ENUM_ACTOR atype)const
	{
		actor_guid lguid(atype, none_area(), none_actordataid());
		return (i64_actorid)(lguid) == (i64_actorid)(*this);
	}

	// actor type 是否无效
	bool actor_guid::is_actortypenone(i64_actorid actorid)
	{
		actor_guid lguid(actorid);
		return lguid.is_actortypenone();
	}

	bool actor_guid::is_actortypenone()const
	{
		return type() == none<i16_actortype>();
	}

	bool actor_guid::is_actorareanone(i64_actorid actorid)
	{
		actor_guid lguid(actorid);
		return lguid.is_actorareanone();
	}

	bool actor_guid::is_actorareanone()const
	{
		return area() == none<i16_area>();
	}

	// actor id 是否无效
	bool actor_guid::is_actoridnone(i64_actorid actorid)
	{
		actor_guid lguid(actorid);
		return lguid.is_actoridnone();
	}

	bool actor_guid::is_actoridnone()const
	{
		return actordataid() == none<i32_actordataid>();
	}

	bool actor_guid::is_single(ENUM_ACTOR atype)
	{
		return atype >= ACTOR_SIGNLE_START && atype < ACTOR_SIGNLE_FINISH;
	}

	i16_area actor_guid::none_area()
	{
		return none<i16_area>();
	}

	ENUM_ACTOR actor_guid::none_type()
	{
		return none<ENUM_ACTOR>();
	}

	i32_actordataid actor_guid::none_actordataid()
	{
		return none<i32_actordataid>();
	}


	bool actor_guid::operator<(int64_t ar)const
	{
		return m_id < ar;
	}

	bool actor_guid::operator<(const actor_guid& r)const
	{
		return m_id < r.m_id;
	}

	bool actor_guid::operator==(int64_t ar)const
	{
		return m_id == ar;
	}

	bool actor_guid::operator==(const actor_guid& r)const
	{
		return m_id == r.m_id;
	}

	bool actor_guid::operator!=(int64_t ar)const
	{
		return m_id != ar;
	}

	bool actor_guid::operator!=(const actor_guid& r)const
	{
		return m_id != r.m_id;
	}

	bool actor_guid::operator>(int64_t ar)const
	{
		return m_id > ar;
	}

	bool actor_guid::operator>(const actor_guid& r)const
	{
		return m_id > r.m_id;
	}
}