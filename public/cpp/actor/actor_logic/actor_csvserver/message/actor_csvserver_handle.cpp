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

#include "actor/actor_logic/actor_csvserver/actor_csvserver.h"
namespace ngl
{
	bool actor_csvserver::handle(const message<np_actor_csv_verify_version>& adata)
	{
		log_error()->print("############actor_reloadcsv_distribute::handle###########");
		auto lparm = adata.get_data();
		auto lpack = adata.get_pack();
		np_actor_reloadcsv pro;
		const auto& lversion = ncsv::all();
		for (auto& [_key, _csv] : lversion)
		{
			auto lpverify = tools::findmap(lparm->m_verify, _key);
			if (lpverify != nullptr && *lpverify != _csv->verify())
			{
				continue;
			}
			reload_csv::readcsv(_key, pro.m_csvcontent[_key]);
		}
		if (pro.m_csvcontent.empty() == false)
		{
			send(lpack->m_id, pro, lpack->m_head.get_request_actor(), id_guid());
		}
		return true;
	}
}//namespace ngl