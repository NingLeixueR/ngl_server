#pragma once

#include "nactor_auto.h"
#include "manage_csv.h"
#include "localtime.h"

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

		void set_id() final
		{
			set_actorid(nguid::make());
		}

		void initdata() final;
	};
}// namespace ngl

mk_formatter(pbdb::db_activity)