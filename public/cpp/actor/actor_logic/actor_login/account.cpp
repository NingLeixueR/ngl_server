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
#include "actor_login.h"
#include "account.h"

namespace ngl
{
	account::account() :
		m_max_accountid(1)
	{
	}

	void account::related_actorid()
	{
		set_actorid(nguid::make());
	}

	void account::initdata()
	{
		log_error()->print("actor_login###loaddb_finish {}", data());
		auto lmapdata = foreach_data();
		for (auto itor = lmapdata.begin();itor!= lmapdata.end();++itor)
		{
			data_modified_return_get(lpdbaccount, itor->second);
			i16_area larea = lpdbaccount->marea();
			const std::string& laccount = lpdbaccount->maccount();
			m_areaofaccount[larea].m_data[laccount] = &itor->second;
			m_max_accountid = std::max(m_max_accountid, nguid::actordataid(lpdbaccount->mid()));
		}
	}

	int32_t& account::max_accountid()
	{
		return m_max_accountid;
	}

	std::map<std::string, data_modified<pbdb::db_account>*>& account::accountbyarea(int area)
	{
		return m_areaofaccount[area].m_data;
	}
}//namespace ngl