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

#include "actor/actor_logic/actor_activity_manage/actor_activity_manage.h"
#include "actor/actor_logic/actor_activity_manage/activity_rank.h"
#include "actor/actor_logic/actor_activity_manage/activity.h"
#include "actor/actor_logic/actor_keyvalue/actor_keyvalue.h"
#include "actor/actor_logic/actor_brief/actor_brief.h"
#include "actor/pb/net.pb.h"

namespace ngl
{
	bool actor_activity_manage::handle(const message<mforward<np_operator_task_response>>& adata)
	{
		i64_actorid roleid = adata.get_data()->identifier();
		const np_operator_task_response* lrecv = adata.get_data()->data();
		int32_t activityid = 0;
		int32_t lindex = 0;
		if (!tools::splite(lrecv->m_msg.c_str(), ":", activityid, lindex))
		{
			return true;
		}
		auto& lactivity = get_activity(activityid);
		if (lactivity == nullptr)
		{
			return true;
		}
		lactivity->recv_task_response(roleid, lindex, lrecv->m_isreceive);
		return true;
	}
	bool actor_activity_manage::handle(const message<np_eevents_logic_rolelogin>& adata)
	{
		const np_eevents_logic_rolelogin* lrecv = adata.get_data();
		for (auto itor = m_activitys.begin(); itor != m_activitys.end(); ++itor)
		{
			itor->second->rolelogin(lrecv->m_actorid);
		}
		return true;
	}
	bool actor_activity_manage::handle(const message<np_eevents_logic_rolevaluechange>& adata)
	{
		const np_eevents_logic_rolevaluechange* lrecv = adata.get_data();
		for (auto itor = m_activitys.begin(); itor != m_activitys.end(); ++itor)
		{
			switch (lrecv->m_type)
			{
			case eevents_logic::eevents_logic_rolelevelchange:
				itor->second->rolelevelchange(lrecv->m_actorid, lrecv->m_beforevalue, lrecv->m_nowvalue);
				break;
			case eevents_logic::eevents_logic_rolegoldchange:
				itor->second->rolegoldchange(lrecv->m_actorid, lrecv->m_beforevalue, lrecv->m_nowvalue);
				break;
			}
		}
		return true;
	}
	bool actor_activity_manage::handle(const message<np_get_rank_response>& adata)
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