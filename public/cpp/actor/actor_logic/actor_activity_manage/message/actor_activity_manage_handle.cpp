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
#include "actor/actor_logic/actor_activity_manage/actor_activity_manage.h"
#include "actor/actor_logic/actor_activity_manage/activity_rank.h"
#include "actor/actor_logic/actor_activity_manage/activity.h"
#include "actor/actor_logic/actor_keyvalue/actor_keyvalue.h"
#include "actor/actor_logic/actor_brief/actor_brief.h"
#include "actor/generated/pb/net.pb.h"
namespace ngl
{
	bool actor_activity_manage::handle([[maybe_unused]] const message<mforward<np_operator_task_response>>& adata)
	{
		const auto* lparm = adata.get_data();
		const i64_actorid lroleid = lparm->identifier();
		const np_operator_task_response* lrecv = lparm->data();
		int32_t lactid = 0;
		int32_t lindex = 0;
		if (!tools::splite(lrecv->m_msg.c_str(), ":", lactid, lindex))
		{
			return true;
		}
		auto& lactivity = get_activity(lactid);
		if (lactivity == nullptr)
		{
			return true;
		}
		lactivity->recv_task_response(lroleid, lindex, lrecv->m_isreceive);
		return true;
	}
	bool actor_activity_manage::handle([[maybe_unused]] const message<np_eevents_logic_rolelogin>& adata)
	{
		const np_eevents_logic_rolelogin* lrecv = adata.get_data();
		for (const auto& lpair : m_activitys)
		{
			lpair.second->rolelogin(lrecv->m_actorid);
		}
		return true;
	}
	bool actor_activity_manage::handle([[maybe_unused]] const message<np_eevents_logic_rolevaluechange>& adata)
	{
		const np_eevents_logic_rolevaluechange* lrecv = adata.get_data();
		for (const auto& lpair : m_activitys)
		{
			const auto& lact = lpair.second;
			switch (lrecv->m_type)
			{
			case eevents_logic::eevents_logic_rolelevelchange:
				lact->rolelevelchange(lrecv->m_actorid, lrecv->m_beforevalue, lrecv->m_nowvalue);
				break;
			case eevents_logic::eevents_logic_rolegoldchange:
				lact->rolegoldchange(lrecv->m_actorid, lrecv->m_beforevalue, lrecv->m_nowvalue);
				break;
			}
		}
		return true;
	}
	bool actor_activity_manage::handle([[maybe_unused]] const message<np_get_rank_response>& adata)
	{
		const np_get_rank_response* lrecv = adata.get_data();
		auto itor = m_activitys.find(lrecv->m_rankid);
		if (itor != m_activitys.end() && itor->second->type() == EActivity::EActivityTopRank)
		{
			((activity_rank*)(itor->second.get()))->handle(*lrecv);
		}
		return true;
	}
}//namespace ngl
