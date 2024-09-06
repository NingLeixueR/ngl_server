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
		rolekv() = default;
		~rolekv() = default;

		//## 可以缓存，防止多次解析vales
		virtual void initdata()
		{
			auto lstream = log_error();
			(*lstream) << std::format("[db_rolekeyvalue load finish] id{}", actorbase()->id_guid()) << std::endl;
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

		bool value(const char* akey, std::string& adata)
		{
			const pbdb::db_rolekeyvalue& ltemp = get_constkv();
			auto itor = ltemp.m_data().find(akey);
			if (itor == ltemp.m_data().end())
			{
				return false;
			}
			adata = itor->second;
			return true;
		}

		template <typename T>
		bool number_value(const char* akey, T& adata)
		{
			std::string ltemp;
			if (value(akey, ltemp) == false)
				return false;
			adata = tools::lexical_cast<T>(ltemp);
			return true;
		}

		bool value(const char* akey, int8_t& adata)
		{
			return number_value(akey, adata);
		}

		bool value(const char* akey, uint8_t& adata)
		{
			return number_value(akey, adata);
		}

		bool value(const char* akey, int32_t& adata)
		{
			return number_value(akey, adata);
		}

		bool value(const char* akey, uint32_t& adata)
		{
			return number_value(akey, adata);
		}

		bool value(const char* akey, int64_t& adata)
		{
			return number_value(akey, adata);
		}

		bool value(const char* akey, uint64_t& adata)
		{
			return number_value(akey, adata);
		}

		bool value(const char* akey, float& adata)
		{
			return number_value(akey, adata);
		}

		bool value(const char* akey, double& adata)
		{
			return number_value(akey, adata);
		}

		template <typename ...ARG>
		bool value(const char* akey, ARG&... arg)
		{
			std::string ltemp;
			if (value(akey, ltemp) == false)
				return false;
			json_read ljread(ltemp.c_str());
			return ljread.read(arg...);
		}

		template <typename T>
		void set_value(const char* akey, T& adata)
		{
			pbdb::db_rolekeyvalue& ltemp = get_kv();
			(*ltemp.mutable_m_data())[akey] = std::format("{}", adata);
		}
	};
}