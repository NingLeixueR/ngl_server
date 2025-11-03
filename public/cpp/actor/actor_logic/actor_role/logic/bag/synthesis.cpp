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
#include "actor_role.h"
#include "synthesis.h"
#include "consume.h"
#include "drop.h"

namespace ngl
{
	bool synthesis::use(actor_role* arole, int32_t aid)
	{
		tab_synthesis* tab = allcsv::tab<tab_synthesis>(aid);
		if (tab == nullptr)
		{
			return false;
		}
		if (consume::use(arole, tab->m_consume, 1) == false)
		{
			log_error()->print(
				"synthesis::use id:{} role:{} consume::use faile {}"
				, aid, arole->id_guid(), tab->m_consume
			);
			return false;
		}
		std::string lsrc = std::format("synthesis::use({},{})", nguid(arole->id_guid()), aid);
		if (arole->get_drop().use(tab->m_generate, 1, arole->id_guid(), lsrc, nullptr) == false)
		{
			log_error()->print(
				"synthesis::use id:{} role:{} drop::use faile {}"
				, aid, arole->id_guid(), tab->m_generate
			);
			return false;
		}
		return true;
	}
}// namespace ngl