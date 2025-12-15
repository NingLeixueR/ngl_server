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
#include "actor_csvserver.h"
namespace ngl
{
	bool actor_csvserver::handle(const message<np_actor_csv_verify_version>& adata)
	{
		log_error()->print("############actor_reloadcsv_distribute::handle###########");
		auto lparm = adata.get_data();
		auto lpack = adata.get_pack();
		np_actor_reloadcsv pro;
		const auto& lversion = allcsv::all();
		for (const auto& apair : lversion)
		{
			auto itor = lparm->m_verify.find(apair.first);
			if (itor != lparm->m_verify.end() && itor->second != apair.second->verify())
			{
				continue;
			}
			reload_csv::readcsv(apair.first, pro.m_csvcontent[apair.first]);
		}
		if (pro.m_csvcontent.empty() == false)
		{
			send(lpack->m_id, pro, lpack->m_head.get_request_actor(), id_guid());
		}
		return true;
	}
}//namespace ngl