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
// File overview: Database-backed storage component for player brief records.

#pragma once

#include "actor/generated/auto/nactor_auto.h"
#include "actor/actor_base/core/actor.h"
#include "tools/tab/csv/ncsv.h"
#include "tools/tools/tools_time.h"

#include <string>
#include <map>

namespace ngl
{
	class actor_brief;

	class briefdb : 
		public tdb_brief::db_modular
	{
		briefdb(const briefdb&) = delete;
		briefdb& operator=(const briefdb&) = delete;

	public:
		briefdb() = default;
		virtual ~briefdb() = default;

		void related_actorid() final
		{
			set_actorid(nguid::make());
		}

		// Add
		void update_brief(const pbdb::db_brief* adata)
		{
			auto& ldata = get(adata->mid());
			*ldata.get() = *adata;
		}

		virtual void initdata()
		{
			log_error()->print("actor_brief###loaddb_finish\n {}", data());
		}

		void update(const std::vector<pbdb::db_brief>& m_vecinfo)
		{
			for (const pbdb::db_brief& item : m_vecinfo)
			{
				update_brief(&item);
			}
		}
	};
}// namespace ngl

mk_formatter(pbdb::db_brief)