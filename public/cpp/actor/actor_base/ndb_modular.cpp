#include "ndb_modular.h"

namespace ngl
{
	ndb_component::ndb_component(pbdb::ENUM_DB aenum) :
		m_actor(nullptr),
		m_id(0),
		m_dbclient(nullptr),
		m_type(aenum)
	{}

	void ndb_component::set_dbclient(ndbclient_base* andbclient)
	{
		m_dbclient = andbclient;
	}

	//# 获取ndbclient基类
	ndbclient_base* ndb_component::get_dbclient()
	{
		return m_dbclient;
	}

	void ndb_component::set(actor_base* aactor)
	{
		m_actor = aactor;
		m_actor->set_db_component(this);
	}

	actor_base* ndb_component::get_actor()
	{
		return m_actor;
	}

	i64_actorid ndb_component::get_actorid()const
	{
		return m_id;
	}

	pbdb::ENUM_DB ndb_component::type()const
	{
		return m_type;
	}

	actor_base* ndb_component::actorbase()
	{
		return m_actor;
	}

	void ndb_component::set_actorid(i64_actorid aactorid)
	{
		m_id = aactorid;
	}

	void ndb_component::related_actorid()
	{
		m_id = m_actor->id_guid();
	}

	void ndb_component::init()
	{
		related_actorid();
		m_actor->add_dbclient(m_dbclient, m_id);
	}

	void ndb_component::create()
	{
		m_dbclient->create(m_id);
	}

	ndbclient_base* ndb_component::dbclientbase()
	{
		return m_dbclient;
	}

	void ndb_component::init_data() 
	{
	}
}//namespace ngl