#pragma once

#include "nactor_auto.h"
#include "manage_csv.h"
#include "localtime.h"
#include "actor.h"

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
		briefdb()
		{
		}

		void related_actorid() final
		{
			set_actorid(nguid::make());
		}

		// 没有就添加
		void update_brief(const pbdb::db_brief* adata)
		{
			data()[adata->m_id()].get() = *adata;
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