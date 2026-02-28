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

#include "actor/actor_logic/actor_csvclient/actor_csvclient.h"
#include "actor/actor_logic/actor_csvserver/actor_csvserver.h"
#include "actor/actor_base/actor_manage.h"

namespace ngl
{
	bool actor_csvclient::handle(const message<np_actor_reloadcsv>& adata)
	{
		for (auto& [key, value] : adata.get_data()->m_csvcontent)
		{
			reload_csv::save(key, value);
		}
		actor_suspend ltemp;
		for (auto& [key, value] : adata.get_data()->m_csvcontent)
		{
			reload_csv::reload(key);
		}
		return true;
	}
}//namespace ngl