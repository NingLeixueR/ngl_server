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
// File overview: Database-backed storage component for the key-value store.

#pragma once

#include "actor/generated/auto/nactor_auto.h"
#include "actor/actor_base/core/actor.h"
#include "tools/tab/csv/ncsv.h"
#include "tools/tools/tools_time.h"

#include <string>
#include <map>

namespace ngl
{
	class actor_keyvalue;

	class keyvaluedb : 
		public tdb_keyvalue::db_modular
	{
		keyvaluedb(const keyvaluedb&) = delete;
		keyvaluedb& operator=(const keyvaluedb&) = delete;

	public:
		keyvaluedb() = default;

		void related_actorid() final
		{
			set_actorid(nguid::make());
		}

		template <typename T>
		bool keyvalue(nguid aid, T& adata)
		{
			data_modified<pbdb::db_keyvalue>& ldata = get(aid);
			MODIFIED_RETURN_CONST(lpdkeyvalue, ldata, false);
			adata = tools::lexical_cast<T>(lpdkeyvalue->mvalue());
			return true;
		}

		template <typename T>
		bool keyvalue_add(nguid aid, const T& adata)
		{
			pbdb::db_keyvalue ldata;
			ldata.set_mid(aid);
			ldata.set_mvalue(tools::lexical_cast<std::string>(adata));
			keyvalue_update(&ldata);
			return true;
		}

		// Add
		void keyvalue_update(const pbdb::db_keyvalue* adata)
		{
			data_modified<pbdb::db_keyvalue>& lpdata = get(adata->mid());
			*lpdata.get() = *adata;
		}

		virtual void initdata()
		{
			log_error()->print("actor_keyvalue###loaddb_finish {}", data());
		}

		void update(const std::vector<pbdb::db_keyvalue>& m_vecinfo)
		{
			for (const pbdb::db_keyvalue& item : m_vecinfo)
			{
				keyvalue_update(&item);
			}
		}
	};
}// namespace ngl

mk_formatter(pbdb::db_keyvalue)