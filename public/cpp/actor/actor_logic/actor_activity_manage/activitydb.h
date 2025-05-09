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
			m_id = -1;
		}

		data_modified<pbdb::db_activity>* finddata(int64_t aactivity)
		{
			auto itor = data().find(aactivity);
			if (itor == data().end())
			{
				return nullptr;
			}
			return &itor->second;
		}

		const pbdb::db_activity* get_constactivity(int64_t aactivity)
		{
			auto lpdata = finddata(aactivity);
			return lpdata == nullptr ? nullptr : &lpdata->getconst();
		}

		pbdb::db_activity* get_activity(int64_t aactivity, bool achange = true)
		{
			auto lpdata = finddata(aactivity);
			return lpdata == nullptr ? nullptr : &lpdata->get();
		}

		void initdata() final;
	};
}// namespace ngl