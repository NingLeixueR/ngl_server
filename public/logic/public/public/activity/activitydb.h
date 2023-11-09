#pragma once

#include "db_modular.h"
#include "localtime.h"
#include "manage_csv.h"
#include <string>
#include <map>

namespace ngl
{
	class actor_manage_activity;
	using activity_db_modular = db_modular<
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

		virtual void initdata()
		{
			LogLocalError("actor_manage_activity###loaddb_finish");
			
			for (const auto& [_roleid, _activity] : data())
			{
				std::cout << "#####################" << std::endl;
				std::cout << "activityid:" << _activity.getconst().m_id() << std::endl;
				char lformat[1024] = { 0 };
				for (int i = 0;i< _activity.getconst().m_datas_size();++i)
				{
					std::cout << ">>>>>>>>>>" << i << "<<<<<<<<<<<<" << std::endl;
					std::cout << "beg:" <<
						localtime::time2str(lformat, 1024, _activity.getconst().m_datas(i).m_beg(), "%y/%m/%d %H:%M%S")
						<< std::endl;
					std::cout << "end:" <<
						localtime::time2str(lformat, 1024, _activity.getconst().m_datas(i).m_end(), "%y/%m/%d %H:%M%S")
						<< std::endl;
					std::cout << "start:" <<
						(_activity.getconst().m_datas(i).m_start()?"true":"false")
						<< std::endl;
					std::cout << "finish:" <<
						(_activity.getconst().m_datas(i).m_finish() ? "true" : "false")
						<< std::endl;
					std::cout << "#####################" << std::endl;
				}
				
			}
		}


		void init_data(int64_t aactivity, int32_t abeg, int32_t aend)
		{
			data_modified<pbdb::db_activity>& ltemp = data()[aactivity];
			ltemp.get().set_m_id(aactivity);
			pbdb::db_activity::data* lpitem = ltemp.get().add_m_datas();
			lpitem->set_m_beg(abeg);
			lpitem->set_m_end(aend);
			lpitem->set_m_start(false);
			lpitem->set_m_finish(false);
		}
	};
}