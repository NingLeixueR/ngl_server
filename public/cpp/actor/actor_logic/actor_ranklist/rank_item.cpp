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
#include "actor_ranklist.h"
#include "rank_item.h"

namespace ngl
{
	const pbdb::db_brief* rank_item::get()const
	{
		static tdb_brief::nsp_cread<actor_ranklist>* lpdb = nsp_instance<tdb_brief::nsp_cread<actor_ranklist>>::nclient(actor_ranklist::actorid());
		if (lpdb == nullptr)
		{
			tools::no_core_dump();
			return nullptr;
		}
		return lpdb->getconst(m_actorid);
	}
}//namespace ngl