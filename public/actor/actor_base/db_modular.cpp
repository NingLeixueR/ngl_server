#include "db_modular.h"

namespace ngl
{
	db_component::db_component(pbdb::ENUM_DB aenum) :
		m_actor(nullptr),
		m_id(0),
		m_dbclient(nullptr),
		m_type(aenum)
	{}

	void db_component::set_dbclient(actor_dbclient_base* adbclient)
	{
		m_dbclient = adbclient;
	}

	void db_component::set(actor_base* aactor)
	{
		m_actor = aactor;
		m_actor->set_db_component(this);
	}

	i64_actorid db_component::id()
	{
		return m_id;
	}

	pbdb::ENUM_DB db_component::type()
	{
		return m_type;
	}

	actor_base* db_component::actorbase()
	{
		return m_actor;
	}

	void db_component::set_id()
	{
		m_id = m_actor->id_guid();
	}

	void db_component::init()
	{
		set_id();
		m_actor->add_dbclient(m_dbclient, m_id);
	}

	void db_component::create()
	{
		m_dbclient->create(m_id);
	}

	actor_dbclient_base* db_component::dbclientbase()
	{
		return m_dbclient;
	}

	// 当数据全部加载后调用
	void db_component::init_data() {}
}