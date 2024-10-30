#include "ndb_modular.h"

namespace ngl
{
	ndb_component::ndb_component(pbdb::ENUM_DB aenum) :
		m_actor(nullptr),
		m_id(0),
		m_dbclient(nullptr),
		m_type(aenum)
	{}

	void ndb_component::set_dbclient(ndbclient_base* adbclient)
	{
		m_dbclient = adbclient;
	}

	void ndb_component::set(actor_base* aactor)
	{
		m_actor = aactor;
		m_actor->set_db_component(this);
		set_logactor(aactor);
	}

	i64_actorid ndb_component::id()const
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

	void ndb_component::set_id()
	{
		m_id = m_actor->id_guid();
	}

	void ndb_component::init()
	{
		set_id();
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