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
// File overview: Declares interfaces for actor login.

#pragma once

#include "actor/auto/nactor_auto.h"

namespace ngl
{
	class account : 
		public tdb_account::db_modular
	{
		account(const account&) = delete;
		account& operator=(const account&) = delete;

		struct accountbyname
		{
			std::map<std::string, data_modified<pbdb::db_account>*> m_data;
		};
		std::map<i16_area, accountbyname> m_areaofaccount;
		
		int32_t m_max_accountid;
	public:
		account();

		void related_actorid() final;

		virtual void initdata();

		int32_t& max_accountid();

		std::map<std::string, data_modified<pbdb::db_account>*>& accountbyarea(int area);
	};
}//namespace ngl

mk_formatter(pbdb::db_account)