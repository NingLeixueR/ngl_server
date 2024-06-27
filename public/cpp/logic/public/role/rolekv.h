#pragma once

#include "nactor_auto.h"
#include "manage_csv.h"
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

		//## 可以缓存，防止多次解析vales
		virtual void initdata()
		{
			auto lstream = log_error();
			(*lstream) 
				<< std::format(
					"[db_rolekeyvalue load finish] id{}", 
					actorbase()->id_guid())
				<< std::endl;
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

		const std::string* get_constkv(const std::string& akey)
		{
			const pbdb::db_rolekeyvalue& ltemp = get_constkv();
			auto itor = ltemp.m_data().find(akey);
			if (itor == ltemp.m_data().end())
			{
				return nullptr;
			}
			return &itor->second;
		}

		std::string* get_kv(const std::string& akey)
		{
			pbdb::db_rolekeyvalue& ltemp = get_kv();
			auto itor = ltemp.mutable_m_data()->find(akey);
			if (itor == ltemp.mutable_m_data()->end())
			{
				return nullptr;
			}
			return &itor->second;
		}

		std::string& operator[](const char* akey)
		{
			pbdb::db_rolekeyvalue& ltemp = get_kv();
			return (*ltemp.mutable_m_data())[akey];
		}
	};
}