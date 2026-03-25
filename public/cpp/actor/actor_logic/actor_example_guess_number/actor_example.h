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
// File overview: Declares interfaces for actor example guess number.

#pragma once 

#include "actor/actor_logic/actor_example_manage/actor_example_manage.h"
#include "actor/actor_logic/actor_create/actor_create.h"
#include "actor/actor_logic/actor_brief/actor_brief.h"
#include "tools/db/sql/mysql/nmysql_manage.h"
#include "tools/db/sql/mysql/nmysql_pool.h"
#include "actor/actor_base/actor_manage.h"
#include "actor/actor_base/ndb_modular.h"
#include "actor/actor_base/ndbclient.h"
#include "tools/db/sql/mysql/nmysql.h"
#include "actor/protocol/nprotocol.h"
#include "actor/tab/ttab_specialid.h"
#include "actor/actor_base/ntimer.h"
#include "tools/db/sql/db_data.h"
#include "actor/pb/example.pb.h"
#include "tools/tab/csv/ncsv.h"
#include "actor/pb/net.pb.h"
#include "net/tcp/ntcp.h"

namespace ngl
{
	template <typename TACTOR>
	class actor_example : 
		public actor
	{
		actor_example(const actor_example&) = delete;
		actor_example& operator=(const actor_example&) = delete;
	protected:
		std::vector<i64_actorid> m_rolesds;
		pbexample::EPLAY_TYPE m_playertype;
	public:
		actor_example(pbexample::EPLAY_TYPE aplayertype, ENUM_ACTOR atype, int32_t aindex, const std::map<int32_t, i64_actorid>& aroleids) :
			actor(
				actorparm
				{
				.m_parm
					{
						.m_type = atype,
						.m_area = tab_self_area,
						.m_id = aindex
					},
				.m_weight = 0x7fffffff,
				}),
			m_playertype(aplayertype)
		{
			m_rolesds.reserve(aroleids.size());
			for (const auto& lpair : aroleids)
			{
				m_rolesds.emplace_back(lpair.second);
			}
		}

		virtual void init()
		{
			std::set<i64_actorid> lset;
			for (i64_actorid aroleid : m_rolesds)
			{
				lset.insert(aroleid);
			}
			std::set<i32_fieldnumber> lfieldset;
			tdb_brief::nsp_cread<actor_example<TACTOR>>::instance_readpart(this, pb_field::field_number<pbdb::db_brief>(lfieldset, "mbase"), lset);
		}

		virtual void erase_actor_before()
		{
			tdb_brief::nsp_cread<actor_example<TACTOR>>::instance(id_guid()).exit();
		}
	};
}//namespace ngl
