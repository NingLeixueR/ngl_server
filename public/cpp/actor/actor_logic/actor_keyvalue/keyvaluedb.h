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

		void related_actorid() final
		{
			set_actorid(nguid::make());
		}

		template <typename T>
		bool keyvalue(nguid aid, T& adata)
		{
			data_modified<pbdb::db_keyvalue>& ldata = get(aid);
			data_modified_return_getconst(lpdkeyvalue, ldata, false);
			adata = tools::lexical_cast<T>(lpdkeyvalue->mvalue());
			return true;
		}

		template <typename T>
		bool add_keyvalue(nguid aid, const T& adata)
		{
			pbdb::db_keyvalue ldata;
			ldata.set_mid(aid);
			ldata.set_mvalue(tools::lexical_cast<std::string>(adata));
			update_keyvalue(&ldata);
			return true;
		}

		// 没有就添加
		void update_keyvalue(const pbdb::db_keyvalue* adata)
		{
			pbdb::db_keyvalue* lpdata = data()[adata->mid()].get();
			if (lpdata == nullptr)
			{
				return;
			}
			*lpdata = *adata;
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