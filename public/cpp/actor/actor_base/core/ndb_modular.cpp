/*
* Copyright (c) [2020-2025] NingLeixueR
* 
* Project name: ngl_server
* Project URL: https://github.com/NingLeixueR/ngl_server
* 
* This file is part of the ngl_server project and is distributed under the MIT License.
* You may use, modify, and distribute this project under the license, including commercial use,
* but you must retain the original copyright and license notice.
* 
* For license details, see the LICENSE file in the project root:
* https://github.com/NingLeixueR/ngl_server/blob/main/LICENSE
*/
// File overview: Implements logic for actor base.

#include "actor/actor_base/core/ndb_modular.h"

namespace ngl
{
	ndb_component::ndb_component(pbdb::ENUM_DB aenum) :
		m_type(aenum)
	{}

	void ndb_component::set_dbclient(ndbclient_base* andbclient)
	{
		m_dbclient = andbclient;
	}

	// Expose the attached DB client so helper code can issue table-specific work.
	ndbclient_base* ndb_component::get_dbclient()
	{
		return m_dbclient;
	}

	void ndb_component::set(actor_base* aactor)
	{
		if (aactor == nullptr)
		{
			log_error()->print("ndb_component::set failed, actor == nullptr");
			return;
		}
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
		if (m_actor == nullptr)
		{
			return;
		}
		m_id = m_actor->id_guid();
	}

	void ndb_component::init()
	{
		if (m_actor == nullptr || m_dbclient == nullptr)
		{
			return;
		}
		// Let the component decide its logical owner id before registering with actor_base.
		related_actorid();
		m_actor->add_dbclient(m_dbclient, m_id);
	}

	void ndb_component::create()
	{
		if (m_dbclient == nullptr)
		{
			return;
		}
		// Create uses the component's currently bound actor id.
		m_dbclient->create(m_id);
	}

	void ndb_component::create(const nguid& aid)
	{
		if (m_dbclient == nullptr)
		{
			return;
		}
		m_dbclient->create(aid);
	}

	ndbclient_base* ndb_component::dbclientbase()
	{
		return m_dbclient;
	}

	void ndb_component::init_data() 
	{
	}
}//namespace ngl
