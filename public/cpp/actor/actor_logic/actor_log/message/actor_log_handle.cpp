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
// File overview: Message handler registration and dispatch for the log actor.
#include "actor/actor_logic/actor_log/actor_log.h"
namespace ngl
{
	bool actor_log::handle([[maybe_unused]] const message<np_logitem>& adata)
	{
		const np_logitem* ldata = adata.get_data();
		auto lplog = tools::findmap(m_logarr, ldata->m_type);
		if (lplog == nullptr)
		{
			return true;
		}
		(*lplog)->printf(ldata);
		return true;
	}
}//namespace ngl