/*
* Copyright (c) [2020-2025] NingLeixueR
* 
* 项目名称：ngl_server
* 项目地址：https://github.com/NingLeixueR/ngl_server
* 
* 本文件是 ngl_server 项目的一部分，遵循 MIT 开源协议发布。
* 您可以按照协议规定自由使用、修改和分发本项目，包括商业用途，
* 但需保留原始版权和许可声明。
* 
* 许可详情参见项目根目录下的 LICENSE 文件：
* https://github.com/NingLeixueR/ngl_server/blob/main/LICENSE
*/
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

		bool value(const char* akey, std::string& adata)
		{			
			data_modified<pbdb::db_rolekeyvalue>& ltemp = get(get_actor()->id_guid());
			const pbdb::db_rolekeyvalue* lpdata = ltemp.getconst();
			if (lpdata == nullptr)
			{
				return false;
			}
			auto& lmap = lpdata->mdata();
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
			data_modified_return_get(lpdbrolekeyvalue, get(get_actor()->id_guid()));
			(*lpdbrolekeyvalue->mutable_mdata())[akey] = std::format("{}", adata);
		}

		template <typename ...ARG>
		bool json_value(const char* akey, ARG&... arg)
		{
			std::string ltemp;
			if (value(akey, ltemp) == false)
			{
				return false;
			}
			njread ljread(ltemp.c_str());
			return njson::read(ljread, arg...);
		}

		template <typename ...ARG>
		bool set_json_value(const char* akey, ARG&... arg)
		{
			njwrite lwrite;
			njson::write(arg...);
			std::string lstr = lwrite.get();
			set_value(akey, lstr);
		}
	};
}//namespace ngl

mk_formatter(pbdb::db_rolekeyvalue)