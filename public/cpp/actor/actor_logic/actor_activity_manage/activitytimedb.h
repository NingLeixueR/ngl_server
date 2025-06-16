#pragma once

#include "nactor_auto.h"
#include "manage_csv.h"
#include "localtime.h"

#include <string>
#include <map>

namespace ngl
{
	class activitytimedb : 
		public tdb_activitytimes::db_modular
	{
		activitytimedb(const activitytimedb&) = delete;
		activitytimedb& operator=(const activitytimedb&) = delete;
	public:
		activitytimedb() = default;

		void set_id() final
		{
			set_actorid(nguid::make());
		}

		void initdata() final;
	};
}// namespace ngl

mk_formatter(pbdb::db_activitytimes)