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
// File overview: Sorted rank list container with insertion, removal, and range queries.

#pragma once

#include "actor/actor_logic/actor_ranklist/rank_item.h"
#include "actor/actor_logic/actor_ranklist/rankset.h"
#include "actor/tab/ttab_specialid.h"
#include "actor/generated/auto/nactor_auto.h"
#include "actor/generated/pb/db.pb.h"

#include <iostream>
#include <sstream>
#include <list>

namespace ngl
{
	class actor_ranklist;

	class ranklist : 
		public tdb_ranklist::db_modular
	{
		ranklist(const ranklist&) = delete;
		ranklist& operator=(const ranklist&) = delete;

		std::map<i64_actorid, rank_item>							m_maprankitem;
		std::map<pbdb::eranklist, std::unique_ptr<rankset_base>>	m_ranks;
	public:
		ranklist();

		void related_actorid() final;

		bool update_value(pbdb::eranklist atype, rank_item& litem, const pbdb::db_brief& abrief, bool afirstsynchronize);

		bool update_value(const pbdb::db_brief& abrief, bool afirstsynchronize);

		void add_data(const pbdb::db_ranklist& aitem);

		virtual void initdata();
				
		bool ranklist_get(i64_actorid aroleid, pbdb::eranklist atype, int32_t apage, pbnet::PROBUFF_NET_RANKLIST_RESPONSE& apro);

		void ranklist_sync(i64_actorid aroleid, pbdb::eranklist atype, int32_t aactivityid, int32_t apage);

		void rank_get(int32_t arankid, std::vector<int64_t>& arolerank);

		void rank_remove(int32_t arankid);

		void rank_add(int32_t arankid);
	};
}// namespace ngl

mk_formatter(pbdb::db_ranklist)