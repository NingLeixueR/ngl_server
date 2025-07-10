#pragma once

#include "nactor_auto.h"
#include "manage_csv.h"
#include "localtime.h"

#include <string>
#include <map>

namespace ngl
{
	class testlua :
		public tdb_testlua::db_modular
	{
		testlua(const testlua&) = delete;
		testlua& operator=(const testlua&) = delete;

	public:
		testlua() = default;

		void related_actorid() final
		{
			set_actorid(nguid::make());
		}

		virtual void initdata()
		{
			log_error()->print("actor_testlua###loaddb_finish ", data());
		}
	};
}// namespace ngl

mk_formatter(pbdb::db_testlua)