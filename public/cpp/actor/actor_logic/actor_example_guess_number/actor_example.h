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

#include "actor/actor_logic/actor_example_manage/actor_example_manage.h"
#include "actor/actor_logic/actor_brief/actor_brief.h"
#include "tools/db/sql/mysql/nmysql_manage.h"
#include "tools/db/sql/mysql/nmysql_pool.h"
#include "actor/actor_base/actor_manage.h"
#include "actor/actor_base/actor_create.h"
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
			for (auto& [_, _actorid] : aroleids)
			{
				m_rolesds.push_back(_actorid);
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