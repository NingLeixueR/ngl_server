#pragma once

#include "nactor_auto.h"
#include "manage_csv.h"
#include "localtime.h"

#include <string>
#include <map>

namespace ngl
{
	class actor_brief;

	class briefdb : public tdb_brief::db_modular
	{
	public:
		briefdb(const briefdb&) = delete;
		briefdb& operator=(const briefdb&) = delete;

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
				(*lstream) << "#####################" << std::endl;
				(*lstream) << "#m_id=" << pair.second.getconst().m_id() << std::endl;
				(*lstream) << "#m_lv=" << pair.second.getconst().m_lv() << std::endl;
				(*lstream) << "#m_moneygold=" << pair.second.getconst().m_moneygold() << std::endl;
				(*lstream) << "#m_moneysilver=" << pair.second.getconst().m_moneysilver() << std::endl;
				(*lstream) << "#m_name=" << pair.second.getconst().m_name() << std::endl;
				(*lstream) << "#m_vip=" << pair.second.getconst().m_vip() << std::endl;
				(*lstream) << "#####################" << std::endl;
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