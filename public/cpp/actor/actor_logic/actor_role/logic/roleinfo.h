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
// File overview: Player profile info queries and display-name management.

#pragma once

#include "actor/actor_logic/actor_events/events_logic.h"
#include "actor/generated/auto/nactor_auto.h"
#include "tools/tab/csv/ncsv.h"
#include "actor/generated/pb/net.pb.h"
#include "actor/generated/pb/db.pb.h"

namespace ngl
{
	class roleinfo : 
		public tdb_role::db_modular
	{
		roleinfo(const roleinfo&) = delete;
		roleinfo& operator=(const roleinfo&) = delete;
	public:
		roleinfo() = default;

		virtual void initdata();

		const i64_actorid roleid();

		pbdb::db_brief* get_brief();
		const pbdb::db_brief* get_constbrief();

		void change_event(eevents_logic type, int abegvalue, int32_t anowvalue);

		// Get/set player level
		int32_t lv();
		void change_lv(int avalues);

		// Get/set VIP level
		int32_t vip();
		void change_vip(int avalues);

		// Translated comment.
		const char* name();
		void rename(const char* aname);

		// Translated comment.
		int32_t gold();
		void change_gold(int avalues);

		// Translated comment.
		int32_t silver();
		void change_silver(int avalues);

		// Translated comment.
		int32_t notalkutc();
		void change_notalkutc(int avalues);
		bool bantalk();
	};
}//namespace ngl

mk_formatter(pbdb::db_role)