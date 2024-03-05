#pragma once

#include "db_modular.h"
#include "localtime.h"
#include "manage_csv.h"
#include <string>
#include <map>

namespace ngl
{
	class actor_brief;

	using brief_db_modular = db_modular<
		EPROTOCOL_TYPE_PROTOCOLBUFF, 
		pbdb::ENUM_DB_BRIEF,
		pbdb::db_brief,
		actor_brief>;

	class briefdb : public brief_db_modular
	{
	public:
		briefdb() :
			brief_db_modular()
		{
		}

		virtual void set_id()
		{
			m_id = -1;
		}


		const pbdb::db_brief* get_constbrief(i64_actorid aroleid)
		{
			auto itor = data().find(aroleid);
			if (itor == data().end())
				return nullptr;
			return &itor->second.getconst();
		}

		pbdb::db_brief* get_activity(i64_actorid aroleid, bool achange = true)
		{
			auto itor = data().find(aroleid);
			if (itor == data().end())
				return nullptr;
			return &itor->second.get(achange);
		}

		virtual void initdata()
		{
			LogLocalError("actor_brief###loaddb_finish")
			
			for (const std::pair<const actor_guid, data_modified<pbdb::db_brief>>& pair : data())
			{
				std::cout << "#####################" << std::endl;
				
				std::cout << "#####################" << std::endl;				
			}
		}

		void update(const std::vector<pbdb::db_brief>& m_vecinfo)
		{
			for (const pbdb::db_brief& item : m_vecinfo)
			{
				data()[item.m_id()].get() = item;
			}
		}
	};
}