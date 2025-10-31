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
#include "actor_testlua2.h"

namespace ngl
{
	actor_testlua2::actor_testlua2() :
		actor(
			actorparm
			{
				.m_parm
				{
					.m_type = ACTOR_TESTLUA2,
					.m_area = tab_self_area,
					.m_manage_dbclient = true,
					.m_enscript = enscript_lua,
				},
				.m_weight = 0x7fffffff,
				.m_broadcast = true,
			})
	{
	}

	ENUM_ACTOR actor_testlua2::actor_type()
	{
		return ACTOR_TESTLUA2;
	}

	i64_actorid actor_testlua2::actorid()
	{
		return nguid::make(actor_type(), tab_self_area, nguid::none_actordataid());
	}

	void actor_testlua2::init()
	{
		// ��DB�ṹ:DB.set(this);

		// ����timer_handle��ʱ��
		/*np_timerparm tparm;
		if (make_timerparm::make_interval(tparm, 2) == false)
		{
			log_error()->print("actor_testlua::init() make_timerparm::make_interval(tparm, 2) == false!!!");
			return;
		}
		set_timer(tparm);
		*/

		// # ��csv����д��lua
		//nscript_data_csv<tab_servers> ltab(ttab_servers::instance().tablecsv());
		//nscript_data_push("csv", ltab);
	}

	void actor_testlua2::erase_actor_before()
	{
	}
	
	void actor_testlua2::loaddb_finish(bool adbishave)
	{
	}

	void actor_testlua2::nregister()
	{
		// ��ʱ��
		actor::register_timer<actor_testlua2>(&actor_testlua2::timer_handle);

		register_script_handle<actor_testlua2>::func<
			np_testlua
		>(true);
	}

	bool actor_testlua2::timer_handle(const message<np_timerparm>& adata)
	{
		return true;
	}

	bool actor_testlua2::handle(const message<np_arg_null>&)
	{
		return true;
	}
}// namespace ngl