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
// File overview: Declares interfaces for actor base.

#pragma once

#include "actor/protocol/nprotocol_template.h"
#include "actor/generated/pb/db.pb.h"
#include "tools/tools.h"

#
#include <map>

namespace ngl
{
	class ndbclient_base;
	class actor_base;
	template <pbdb::ENUM_DB DBTYPE, typename TDBTAB, typename TACTOR>
	class ndbclient;

	class nmanage_dbclient
	{
		using tmap_dbclient = std::map<pbdb::ENUM_DB, ndbclient_base*>;
		actor_base* m_actor = nullptr; // Owning actor.
		tmap_dbclient										m_typedbclientmap;					// DB clients still waiting for load completion.
		tmap_dbclient										m_dbclientmap;						// DB clients that have finished loading.
		std::function<void(pbdb::ENUM_DB, enum_dbstat)>		m_loadfinishfun;					// Actor callback fired on partial and full load completion.
	public:
		explicit nmanage_dbclient(actor_base* aactor) :
			m_actor(aactor)
		{}

		// Add a DB client and immediately initialize its actor binding.
		void add(ndbclient_base* adbclient, const nguid& aid);

		// Register the actor-level callback invoked when one DB table finishes loading.
		void set_loadfinish_function(const std::function<void(pbdb::ENUM_DB, enum_dbstat)>& afun);

		// Initialize one concrete dbclient against the owning actor and target guid.
		void init(ndbclient_base* adbclient, actor_base* aactor, const nguid& aid);

		// Promote fully loaded DB clients and fire finalization hooks once all are ready.
		bool on_load_finish(pbdb::ENUM_DB atype, enum_dbstat astat);

		// Return true when every registered dbclient has completed loading.
		bool isloadfinish()const;

		// Retrieve a typed dbclient from the "loading" or "loaded" map.
		template <pbdb::ENUM_DB ENUM, typename TDATA, typename TACTOR>
		ndbclient<ENUM, TDATA, TACTOR>* data(bool aloadfinish);

		// Flush updates, flush deletes, and mirror loaded data into the scripting runtime.
		void save();

		void del();

		void nscript_push_data();
	};

}//namespace ngl
