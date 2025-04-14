#pragma once

#include "nactor_auto.h"
#include "manage_csv.h"
#include "localtime.h"
#include "actor.h"

#include <string>
#include <map>

namespace ngl
{
	class actor_keyvalue;

	class keyvaluedb : public tdb_keyvalue::db_modular
	{
		keyvaluedb(const keyvaluedb&) = delete;
		keyvaluedb& operator=(const keyvaluedb&) = delete;

	public:
		keyvaluedb()
		{
		}

		virtual void set_id()
		{
			m_id = -1;
		}

		const pbdb::db_keyvalue* get_constkeyvaluedb(i64_actorid aroleid)
		{
			auto itor = data().find(aroleid);
			if (itor == data().end())
			{
				return nullptr;
			}
			return &itor->second.getconst();
		}

		pbdb::db_keyvalue* get_keyvaluedb(i64_actorid aroleid, bool achange = true)
		{
			auto itor = data().find(aroleid);
			if (itor == data().end())
			{
				return nullptr;
			}
			return &itor->second.get(achange);
		}

		// û�о����
		void update_keyvalue(const pbdb::db_keyvalue* adata)
		{
			data()[adata->m_id()].get() = *adata;
		}

		virtual void initdata()
		{
			auto lstream = log_error();
			(*lstream) << "actor_keyvalue###loaddb_finish" << std::endl;
			for (const auto& [_id, _data] : data())
			{
				(*lstream) << std::format("key/value = {}/{}", _data.getconst().m_id(), _data.getconst().m_value()) << std::endl;
			}
			lstream->print("");
		}

		void update(const std::vector<pbdb::db_keyvalue>& m_vecinfo)
		{
			for (const pbdb::db_keyvalue& item : m_vecinfo)
			{
				update_keyvalue(&item);
			}
		}
	};
}// namespace ngl