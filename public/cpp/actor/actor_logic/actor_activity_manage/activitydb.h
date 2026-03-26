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
// File overview: Declares interfaces for actor activity manage.

#pragma once

#include "actor/generated/auto/nactor_auto.h"
#include "tools/tab/csv/ncsv.h"
#include "tools/localtime.h"

#include <string>
#include <map>

namespace ngl
{
	class activitydb : 
		public tdb_activity::db_modular
	{
		activitydb(const activitydb&) = delete;
		activitydb& operator=(const activitydb&) = delete;
	public:
		activitydb() = default;

		void related_actorid() final
		{
			set_actorid(nguid::make());
		}

		void initdata() final;
	};
}// namespace ngl

mk_formatter(pbdb::db_activity)