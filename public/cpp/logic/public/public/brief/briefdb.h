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

	class briefdb : public tdb_brief::db_modular
	{
		briefdb(const briefdb&) = delete;
		briefdb& operator=(const briefdb&) = delete;

	public:
		briefdb()
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

		pbdb::db_brief* get_brief(i64_actorid aroleid, bool achange = true)
		{
			auto itor = data().find(aroleid);
			if (itor == data().end())
				return nullptr;
			return &itor->second.get(achange);
		}

		// 没有就添加
		void update_brief(const pbdb::db_brief* adata)
		{
			data()[adata->m_id()].get() = *adata;
		}

		virtual void initdata()
		{
			auto lstream = log_error();
			(*lstream) << "actor_brief###loaddb_finish" << std::endl;
			for (const std::pair<const nguid, data_modified<pbdb::db_brief>>& pair : data())
			{
				const pbdb::db_brief& lbrief = pair.second.getconst();
				std::format(R"(
++++++++++++++++++++++
+m_id={}
+m_lv={}
+m_moneygold={}
+m_moneysilver={}
+m_name={}
+m_vip={}
++++++++++++++++++++++
)", 
					lbrief.m_id(),
					lbrief.m_lv(),
					lbrief.m_moneygold(),
					lbrief.m_moneysilver(),
					lbrief.m_name(),
					lbrief.m_vip()
					);
			}
			(*lstream).print("");
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