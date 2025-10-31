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
#include "nprotocol.h"
#include "actor_log.h"

namespace ngl
{
	bool nactor_logitem::m_init = false;

	void nactor_logitem::send(std::shared_ptr<np_logitem> pro, bool aisnet)
	{
		if (aisnet)
		{
			// # ACTOR_TYPE # ELOG_TYPE
			actor::send_actor(actor_log::actorid(ttab_servers::instance().tab()->m_log), nguid::make(), pro);
		}
		else
		{
			actor::send_actor(actor_log::actorid(nconfig::m_tid), nguid::make(), pro);
		}
	}
}//namespace ngl