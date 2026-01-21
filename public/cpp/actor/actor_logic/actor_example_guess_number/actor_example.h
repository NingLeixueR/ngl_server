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

#include "actor_example_manage.h"
#include "ttab_specialid.h"
#include "actor_manage.h"
#include "actor_create.h"
#include "actor_brief.h"
#include "ndb_modular.h"
#include "example.pb.h"
#include "ndbclient.h"
#include "nprotocol.h"
#include "db_manage.h"
#include "db_data.h"
#include "db_pool.h"
#include "net.pb.h"
#include "ntimer.h"
#include "ncsv.h"
#include "net.h"
#include "db.h"

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
			for (const auto& apair : aroleids)
			{
				m_rolesds.push_back(apair.second);
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