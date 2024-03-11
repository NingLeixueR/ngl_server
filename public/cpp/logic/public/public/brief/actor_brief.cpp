#include "actor_brief.h"
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
						.m_area = ttab_servers::tab()->m_area
						//.m_id = nconfig::m_nodeid,//设置此值说明每个进程都有一个聊天actor
					},
				.m_weight = 0x7fffffff,
			})
	{
	}


	void actor_brief::nregister()
	{
		// 协议注册
		register_actor<EPROTOCOL_TYPE_CUSTOM, actor_brief>(
			false
			, dregister_fun_handle(actor_brief, np_actor_roleinfo)
			);
	}
}