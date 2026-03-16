/*
* Copyright (c) [2020-2025] NingLeixueR
* 
* Project name: ngl_server
* Project URL: https://github.com/NingLeixueR/ngl_server
* 
* This file is part of the ngl_server project and is distributed under the MIT License.
* You may use, modify, and distribute this project under the license, including commercial use,
* but you must retain the original copyright and license notice.
* 
* For license details, see the LICENSE file in the project root:
* https://github.com/NingLeixueR/ngl_server/blob/main/LICENSE
*/
// File overview: Declares interfaces for logic.

#pragma once

#include "actor/auto/nactor_auto.h"
#include "tools/tab/csv/ncsv.h"
#include "actor/pb/net.pb.h"
#include "actor/pb/db.pb.h"

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
		bool get_value(const char* akey, const std::array<const char*, sizeof...(ARG)>& akeys, ARG&... arg)
		{
			const char* lvalue = get_value(akey);
			if (lvalue == nullptr)
			{
				return false;
			}
			if (std::string_view(lvalue).empty())
			{
				return false;
			}
			ncjson ljread(lvalue);
			if (!ljread.parsecheck())
			{
				return false;
			}
			return njson::pop(ljread, akeys, arg...);
		}

		template <typename ...ARG>
		void set_value(const char* akey, const std::array<const char*, sizeof...(ARG)>& akeys, const ARG&... arg)
		{
			ncjson lwrite(get_value(akey));
			njson::push(lwrite, akeys, arg...);
			set_value(akey, lwrite.str());
		}
	};
}//namespace ngl

mk_formatter(pbdb::db_rolekeyvalue)