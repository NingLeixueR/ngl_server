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
// File overview: Implements logic for auto.


 // Note [makeproto tool-generated file, do not edit manually]
 // Createtime[2026-03-14 11:11:12]
#include "actor/auto/auto_actor_enum.h"
#include "actor/auto/nprotocol_auto.h"
#include "actor/auto/nactor_auto.h"

namespace ngl
{
	void tdb::tdb_init(bool ainstance)
	{
		tdb_account::init(ainstance);
		tdb_activity::init(ainstance);
		tdb_activitytimes::init(ainstance);
		tdb_bag::init(ainstance);
		tdb_brief::init(ainstance);
		tdb_family::init(ainstance);
		tdb_familyer::init(ainstance);
		tdb_friends::init(ainstance);
		tdb_keyvalue::init(ainstance);
		tdb_mail::init(ainstance);
		tdb_notice::init(ainstance);
		tdb_ranklist::init(ainstance);
		tdb_role::init(ainstance);
		tdb_rolekv::init(ainstance);
		tdb_task::init(ainstance);
		tdb_testlua::init(ainstance);
    }
}//namespace ngl