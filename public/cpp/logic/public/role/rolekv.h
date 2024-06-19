#pragma once

#include "nactor_auto.h"
#include "manage_csv.h"
#include "malloc_buff.h"
#include "net.pb.h"
#include "db.pb.h"

namespace ngl
{
	class rolekv : public tdb_rolekv::db_modular
	{
		rolekv(const rolekv&) = delete;
		rolekv& operator=(const rolekv&) = delete;

	public:
		rolekv()
		{}

		~rolekv() {}

		virtual void initdata()
		{
			auto lstream = log_error();
			(*lstream) << "#[rolekv]	#[load finish]" << std::endl;
			(*lstream) << "#[id]		#[" << actorbase()->id_guid() << "]" << std::endl;
			(*lstream).print("");
		}

		const pbdb::db_rolekeyvalue& get_constkv()
		{
			return db()->getconst();
		}

		pbdb::db_rolekeyvalue& get_kv()
		{
			return db()->get();
		}

		//## 可以缓存，防止多次解析vales


	};


}