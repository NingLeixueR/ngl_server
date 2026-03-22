/*
* Copyright (c) [2020-2025] NingLeixueR
* 
* Project name: ngl_server
* Project URL: https://github.com/NingLeixueR/ngl_server
* 
* This file is part of the ngl_server project and is distributed under the MIT License.
* You may use, modify, and distribute this project under the license, including commercial use,
* but you must retain the original copyright and license notice.
* 
* For license details, see the LICENSE file in the project root:
* https://github.com/NingLeixueR/ngl_server/blob/main/LICENSE
*/
// File overview: Implements logic for actor login.


#include "actor/actor_logic/actor_login/actor_login.h"
#include "actor/actor_logic/actor_login/account.h"

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
		auto& lmapdata = foreach_data();
		for (auto itor = lmapdata.begin();itor!= lmapdata.end();++itor)
		{
			MODIFIED_RETUAN(lpdbaccount, itor->second);
				i16_area larea = static_cast<i16_area>(lpdbaccount->marea());
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
		return m_areaofaccount[static_cast<i16_area>(area)].m_data;
	}
}//namespace ngl
