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

#include "actor/actor_base/core/nmanage_dbclient.h"
#include "actor/actor_base/core/actor_base.h"
#include "actor/actor_base/core/ndbclient.h"

namespace ngl
{
	void nmanage_dbclient::add(ndbclient_base* adbclient, const nguid& aid)
	{
		auto [_, inserted] = m_typedbclientmap.try_emplace(adbclient->type(), adbclient);
		if (!inserted)
		{
			tools::no_core_dump();
			return;
		}
		// Each dbclient starts in the "loading" map and migrates to m_dbclientmap once ready.
		init(adbclient, m_actor, aid);
	}

	void nmanage_dbclient::set_loadfinish_function(const std::function<void(pbdb::ENUM_DB, enum_dbstat)>& afun)
	{
		m_loadfinishfun = afun;
	}

	void nmanage_dbclient::init(ndbclient_base* adbclient, actor_base* aactor, const nguid& aid)
	{
		adbclient->init(this, aactor, aid);
	}

	bool nmanage_dbclient::on_load_finish(pbdb::ENUM_DB atype, enum_dbstat astat)
	{
		if (m_typedbclientmap.empty())
		{
			log_error()->print("on_load_finish m_typedbclientmap.empty()");
			return false;
		}

		// Notify the actor that one concrete DB table reported progress.
		m_loadfinishfun(atype, astat);

		for (auto itor = m_typedbclientmap.begin(); itor != m_typedbclientmap.end();)
		{
			if (!itor->second->isload())
			{
				++itor;
				continue;
			}
			// Loaded clients move into the stable map used by save()/del()/script sync.
			m_dbclientmap.insert_or_assign(itor->first, itor->second);
			itor = m_typedbclientmap.erase(itor);
		}

		if (!m_typedbclientmap.empty())
		{
			log_error()->print("on_load_finish !m_typedbclientmap.empty()");
			return false;
		}

		m_actor->db_component_init_data();

		// 1. Loading can mark rows dirty; clear those flags before gameplay starts mutating data.
		for (const auto& lpair : m_dbclientmap)
		{
			lpair.second->clear_modified();
		}

		// 2. Mirror the freshly loaded state into the script runtime.
		nscript_push_data();

		// 3. Signal the "all DB ready" stage to both C++ and script callers.
		if (m_loadfinishfun != nullptr)
		{
			m_loadfinishfun(pbdb::ENUM_DB_ALL, astat);
		}
		m_actor->nscript_db_loadfinish();
		return true;
	}

	bool nmanage_dbclient::isloadfinish()const
	{
		return m_typedbclientmap.empty();
	}

	void nmanage_dbclient::save()
	{
		for (const auto& lpair : m_dbclientmap)
		{
			lpair.second->savedb();
		}
	}

	void nmanage_dbclient::del()
	{
		for (const auto& lpair : m_dbclientmap)
		{
			lpair.second->deldb();
		}
	}

	void nmanage_dbclient::nscript_push_data()
	{
		for (const auto& lpair : m_dbclientmap)
		{
			lpair.second->nscript_push_data();
		}
	}
}//namespace ngl
