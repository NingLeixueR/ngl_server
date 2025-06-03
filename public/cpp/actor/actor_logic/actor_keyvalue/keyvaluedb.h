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

	class keyvaluedb : 
		public tdb_keyvalue::db_modular
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

		data_modified<pbdb::db_keyvalue>* get_valuedb(i64_actorid aroleid)
		{
			auto itor = data().find(aroleid);
			if (itor == data().end())
			{
				return nullptr;
			}
			return &itor->second;
		}


		template <typename T>
		bool keyvalue(nguid aid, T& adata)
		{
			data_modified<pbdb::db_keyvalue>* lp = get(aid);
			if (lp == nullptr)
			{
				return false;
			}
			adata = tools::lexical_cast<T>(lp->getconst().m_value());
			return true;
		}

		template <typename T>
		bool add_keyvalue(nguid aid, const T& adata)
		{
			pbdb::db_keyvalue ldata;
			ldata.set_m_id(aid);
			ldata.set_m_value(tools::lexical_cast<std::string>(adata));
			update_keyvalue(&ldata);
			return true;
		}


		// 没有就添加
		void update_keyvalue(const pbdb::db_keyvalue* adata)
		{
			data()[adata->m_id()].get() = *adata;
		}

		virtual void initdata()
		{
			log_error()->print("actor_keyvalue###loaddb_finish {}", data());
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

mk_formatter(pbdb::db_keyvalue)