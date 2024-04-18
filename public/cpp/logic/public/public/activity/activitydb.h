#pragma once

#include "ndb_modular.h"
#include "manage_csv.h"
#include "localtime.h"

#include <string>
#include <map>

namespace ngl
{
	class actor_manage_activity;
	using activity_db_modular = ndb_modular<
		EPROTOCOL_TYPE_PROTOCOLBUFF, 
		pbdb::ENUM_DB_ACTIVITY,
		pbdb::db_activity,
		actor_manage_activity>;

	class activitydb : public activity_db_modular
	{
	public:
		activitydb() :
			activity_db_modular()
		{
		}

		virtual void set_id()
		{
			m_id = -1;
		}

		const pbdb::db_activity* get_constactivity(int64_t aactivity)
		{
			auto itor = data().find(aactivity);
			if (itor == data().end())
				return nullptr;
			return &itor->second.getconst();
		}

		pbdb::db_activity* get_activity(int64_t aactivity, bool achange = true)
		{
			auto itor = data().find(aactivity);
			if (itor == data().end())
				return nullptr;
			return &itor->second.get(achange);
		}

		virtual void initdata();
	};
}// namespace ngl