#pragma once

#include "nactor_auto.h"
#include "manage_csv.h"
#include "net.pb.h"
#include "db.pb.h"

namespace ngl
{
	class rolekv : 
		public tdb_rolekv::db_modular
	{
		rolekv(const rolekv&) = delete;
		rolekv& operator=(const rolekv&) = delete;

	public:
		rolekv() = default;
		~rolekv() = default;

		// # 可以缓存，防止多次解析vales
		virtual void initdata()
		{
			log_error()->print("[db_rolekeyvalue load finish] {}", data());
		}

		data_modified<pbdb::db_rolekeyvalue>& get_rolekv();

		bool value(const char* akey, std::string& adata)
		{
			data_modified<pbdb::db_rolekeyvalue>& ltemp = get_rolekv();
			auto& lmap = ltemp.getconst().m_data();
			auto itor = lmap.find(akey);
			if (itor == lmap.end())
			{
				return false;
			}
			adata = itor->second;
			return true;
		}

	private:
		template <typename T>
		bool number_value(const char* akey, T& adata)
		{
			std::string ltemp;
			if (value(akey, ltemp) == false)
			{
				return false;
			}
			adata = tools::lexical_cast<T>(ltemp);
			return true;
		}
	public:
		bool value(const char* akey, int8_t& adata);
		bool value(const char* akey, uint8_t& adata);
		bool value(const char* akey, int32_t& adata);
		bool value(const char* akey, uint32_t& adata);
		bool value(const char* akey, int64_t& adata);
		bool value(const char* akey, uint64_t& adata);
		bool value(const char* akey, float& adata);
		bool value(const char* akey, double& adata);

		template <typename T>
		void set_value(const char* akey, T& adata)
		{
			pbdb::db_rolekeyvalue& ltemp = get_rolekv().get();
			(*ltemp.mutable_m_data())[akey] = std::format("{}", adata);
		}

		template <typename ...ARG>
		bool json_value(const char* akey, ARG&... arg)
		{
			std::string ltemp;
			if (value(akey, ltemp) == false)
			{
				return false;
			}
			json_read ljread(ltemp.c_str());
			return ljread.read(arg...);
		}

		template <typename ...ARG>
		bool set_json_value(const char* akey, ARG&... arg)
		{
			json_write lwrite;
			lwrite.write(arg...);
			std::string lstr;
			lwrite.get(lstr);
			set_value(akey, lstr);
		}
	};
}//namespace ngl

mk_formatter(pbdb::db_rolekeyvalue)