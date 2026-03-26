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
// File overview: Implements message handlers for message.
#include "actor/actor_logic/actor_csvclient/actor_csvclient.h"
#include "actor/actor_logic/actor_csvserver/actor_csvserver.h"
#include "actor/actor_base/core/actor_manage.h"
namespace ngl
{
	bool actor_csvclient::handle([[maybe_unused]] const message<np_actor_reloadcsv>& adata)
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
