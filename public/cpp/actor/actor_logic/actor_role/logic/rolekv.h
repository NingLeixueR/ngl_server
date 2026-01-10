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

		virtual void initdata();

		const char* get_value(const char* akey);

		void set_value(const char* akey, const char* adata);
	public:
		template <typename ...ARG>
		bool get_value(const char* akey, const std::vector<const char*>& akeys, ARG&... arg)
		{
			const char* lvalue = get_value(akey);
			if (lvalue == nullptr)
			{
				return false;
			}
			ncjson ljread(lvalue);
			return njson::pop(ljread.json(), akeys, arg...);
		}

		template <typename ...ARG>
		void set_value(const char* akey, const std::vector<const char*>& akeys, const ARG&... arg)
		{
			ncjson lwrite(get_value(akey));
			njson::push(lwrite.json(), akeys, arg...);
			set_value(akey, lwrite.str());
		}
	};
}//namespace ngl

mk_formatter(pbdb::db_rolekeyvalue)