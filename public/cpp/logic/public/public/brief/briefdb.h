#pragma once

#include "ndb_modular.h"
#include "manage_csv.h"
#include "localtime.h"

#include <string>
#include <map>

namespace ngl
{
	class actor_brief;

	using brief_db_modular = ndb_modular<
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
			LogLocalStreamError(lstream);
			lstream << "actor_brief###loaddb_finish" << std::endl;
			for (const std::pair<const nguid, data_modified<pbdb::db_brief>>& pair : data())
			{
				lstream << "#####################" << std::endl;
				lstream << "#m_id=" << pair.second.getconst().m_id() << std::endl;
				lstream << "#m_lv=" << pair.second.getconst().m_lv() << std::endl;
				lstream << "#m_moneygold=" << pair.second.getconst().m_moneygold() << std::endl;
				lstream << "#m_moneysilver=" << pair.second.getconst().m_moneysilver() << std::endl;
				lstream << "#m_name=" << pair.second.getconst().m_name() << std::endl;
				lstream << "#m_vip=" << pair.second.getconst().m_vip() << std::endl;
				lstream << "#####################" << std::endl;
			}
			lstream.print();
		}

		void update(const std::vector<pbdb::db_brief>& m_vecinfo)
		{
			for (const pbdb::db_brief& item : m_vecinfo)
			{
				data()[item.m_id()].get() = item;
			}
		}
	};
}// namespace ngl