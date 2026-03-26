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
// File overview: Declares interfaces for bag.

#pragma once

#include "actor/actor_logic/actor_role/logic/bag/itme.h"
#include "actor/actor_logic/actor_role/logic/remakes.h"
#include "actor/generated/auto/nactor_auto.h"
#include "actor/generated/pb/db.pb.h"

namespace ngl
{
	class autoitem;

	class bag :
		public tdb_bag::db_modular
	{
		std::map<int, pbdb::item*> m_stackitems;				// Key tid item
		std::map<int, pbdb::item*> m_nostackitems;				// Key id item
		std::unique_ptr<autoitem>  m_autoitem;
	public:
		bag();

		virtual void initdata();

		pbdb::item* add(pbdb::item& aitem);
	
		bool add_item(int32_t atid, int32_t acount);
		bool add_item(const std::map<int32_t, int32_t>& amap);
		bool add_item(std::vector<pbdb::item>& avec);

		// Delete item
		bool dec_item(int32_t atid, int32_t acount);
		// Delete item
		bool dec_item(int32_t aid);
		
		bool checkbytid(int32_t atid, int32_t acount);
		bool checkbyid(int32_t aid);

		void sync_client();

		void print_bi(int32_t aitemid, int32_t atid, int32_t acount, bool aisadd = true);
	};
}// namespace ngl

mk_formatter(pbdb::db_bag)