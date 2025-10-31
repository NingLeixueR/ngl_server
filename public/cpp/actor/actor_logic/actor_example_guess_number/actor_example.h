/*
* Copyright (c) [2020-2025] NingLeixueR
* 
* ��Ŀ���ƣ�ngl_server
* ��Ŀ��ַ��https://github.com/NingLeixueR/ngl_server
* 
* ���ļ��� ngl_server ��Ŀ��һ���֣���ѭ MIT ��ԴЭ�鷢����
* �����԰���Э��涨����ʹ�á��޸ĺͷַ�����Ŀ��������ҵ��;��
* ���豣��ԭʼ��Ȩ�����������
* 
* �������μ���Ŀ��Ŀ¼�µ� LICENSE �ļ���
* https://github.com/NingLeixueR/ngl_server/blob/main/LICENSE
*/
#pragma once 
#include "actor_example_manage.h"
#include "ttab_specialid.h"
#include "actor_manage.h"
#include "actor_create.h"
#include "actor_brief.h"
#include "ndb_modular.h"
//#include "nsp_server.h"
//#include "nsp_client.h"
#include "manage_csv.h"
#include "ndbclient.h"
#include "nprotocol.h"
#include "db_manage.h"
#include "db_data.h"
#include "db_pool.h"
#include "ntimer.h"
#include "net.h"
#include "db.h"

#include "example.pb.h"
#include "net.pb.h"


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
		actor_example(pbexample::EPLAY_TYPE aplayertype, ENUM_ACTOR atype, int32_t aindex, const std::map<int32_t, i64_actorid>& aroleids) :actor(
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
			tdb_brief::nsp_cread<actor_example<TACTOR>>::instance_readpart(this, lset);
		}

		virtual void erase_actor_before()
		{
			tdb_brief::nsp_cread<actor_example<TACTOR>>::instance(id_guid()).exit();
		}
	};
}//namespace ngl