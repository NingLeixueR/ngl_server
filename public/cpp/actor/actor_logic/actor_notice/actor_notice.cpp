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
#include "actor_notice.h"

namespace ngl
{
	actor_notice::actor_notice() :
		actor(
			actorparm
			{
				.m_parm
				{
					.m_type = ACTOR_NOTICE,
					.m_area = tab_self_area,
					.m_manage_dbclient = true,
				},
				.m_weight = 0x7fffffff,
				.m_broadcast = true,
			})
	{
	}

	ENUM_ACTOR actor_notice::actor_type()
	{
		return ACTOR_NOTICE;
	}

	i64_actorid actor_notice::actorid()
	{
		return nguid::make(actor_type(), tab_self_area, nguid::none_actordataid());
	}

	void actor_notice::init()
	{
		// ��DB�ṹ:DB.set(this);
		m_notice.set(this);

		// ����timer_handle��ʱ��
		/*np_timerparm tparm;
		if (make_timerparm::make_interval(tparm, 2) == false)
		{
			log_error()->print("actor_notice::init() make_timerparm::make_interval(tparm, 2) == false!!!");
			return;
		}
		set_timer(tparm);
		*/
	}

	void actor_notice::loaddb_finish(bool adbishave) 
	{
	}

	void actor_notice::nregister()
	{
		// ��ʱ��
		actor::register_timer<actor_notice>(&actor_notice::timer_handle);

		// ���Զ���np_��Ϣ
		register_handle<actor_notice>::func<
			mforward<np_gm>
			, np_actor_addnotice
			, mforward<pbnet::PROBUFF_NET_NOTICE>
		>(false);
	}

	bool actor_notice::sendnotice(const std::string& anotice, int32_t astarttime, int32_t afinishtime)
	{
		auto pro = std::make_shared<np_actor_addnotice>();
		pro->m_notice = anotice;
		pro->m_starttime = astarttime;
		pro->m_finishtime = afinishtime;
		actor::send_actor(actorid(), actor_notice::actorid(), pro);
		return true;
	}

	bool actor_notice::timer_handle(const message<np_timerparm>& adata)
	{
		return true;
	}

	bool actor_notice::handle(const message<np_arg_null>&)
	{
		return true;
	}
}//namespace ngl