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
#include "actor_brief.h"
#include "nsp_server.h"
#include "net.pb.h"

namespace ngl
{
	actor_brief::actor_brief() :
		actor(
			actorparm
			{
				.m_parm
					{
						.m_type = ACTOR_BRIEF,
						.m_area = tab_self_area,
						.m_manage_dbclient = true
					},
				.m_weight = 0x7fffffff,
				.m_broadcast = true,
			})
	{
	}

	ENUM_ACTOR actor_brief::actor_type()
	{
		return ACTOR_BRIEF;
	}

	i64_actorid actor_brief::actorid()
	{
		return nguid::make(actor_type(), tab_self_area, nguid::none_actordataid());
	}

	void actor_brief::init()
	{
		// ��DB�ṹ:DB.set(this);
		m_briefdb.set(this);

		// ����timer_handle��ʱ��
		/*np_timerparm tparm;
		if (make_timerparm::make_interval(tparm, 2) == false)
		{
			log_error()->print("actor_brief::init() make_timerparm::make_interval(tparm, 2) == false!!!");
			return;
		}
		set_timer(tparm);
		*/

		tdb_brief::nsp_ser::init(&m_briefdb);
	}

	void actor_brief::loaddb_finish(bool adbishave)
	{
		log_error()->print("actor_brief::loaddb_finish({})", adbishave?"true":"false");
	}

	void actor_brief::nregister()
	{
		// ��ʱ��
		actor::register_timer<actor_brief>(&actor_brief::timer_handle);

		// ���Զ���np_��Ϣ
		register_handle<actor_brief>::func<
		>(false);
	}
	
	bool actor_brief::timer_handle(const message<np_timerparm>& adata)
	{
		return true;
	}

	bool actor_brief::handle(const message<np_arg_null>&)
	{
		return true;
	}
}//namespace ngl