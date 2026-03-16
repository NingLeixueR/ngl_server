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
#include "actor/pb/db.pb.h"
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
		actor_base* m_actor = nullptr;
		tmap_dbclient										m_typedbclientmap;
		tmap_dbclient										m_dbclientmap;						// Load
		std::function<void(pbdb::ENUM_DB, enum_dbstat)>		m_loadfinishfun;					// Bool dbdatabasewhether this data

		void foreach_function(const std::function<void(ndbclient_base*)>& afun);
	public:
		explicit nmanage_dbclient(actor_base* aactor) :
			m_actor(aactor)
		{}

		void add(ndbclient_base* adbclient, const nguid& aid);

		void set_loadfinish_function(const std::function<void(pbdb::ENUM_DB, enum_dbstat)>& afun);

		void init(ndbclient_base* adbclient, actor_base* aactor, const nguid& aid);

		bool on_load_finish(pbdb::ENUM_DB atype, enum_dbstat astat);

		bool isloadfinish()const;

		template <pbdb::ENUM_DB ENUM, typename TDATA, typename TACTOR>
		ndbclient<ENUM, TDATA, TACTOR>* data(bool aloadfinish);

		void save();

		void del();

		void nscript_push_data();
	};

}//namespace ngl