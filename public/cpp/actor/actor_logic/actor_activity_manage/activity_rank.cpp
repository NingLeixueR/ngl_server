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
// File overview: Implements rank-based activity scoring and reward distribution.


#include "actor/actor_logic/actor_activity_manage/actor_activity_manage.h"
#include "actor/actor_logic/actor_activity_manage/activity_rank.h"
#include "actor/actor_logic/actor_ranklist/actor_ranklist.h"

namespace ngl
{
	void activity_rank::start()
	{
		auto pro = std::make_shared<np_activityrank_operator>();
		pro->m_iscreate = true;
		pro->m_rankid = m_tab->m_id;
		actor::send_actor(actor_ranklist::actorid(), nguid::make(), pro);
		activity::start();
	}

	void activity_rank::finish()
	{
		// Send rank rewards
		// 1. Request player rank info from the rank module
		// 2. Distribute rewards based on rank
		auto pro = std::make_shared<np_get_rank>();
		pro->m_rankid = m_tab->m_id;

		actor::send_actor(actor_ranklist::actorid(), actor_activity_manage::actorid(), pro);
		return;
	}

	void activity_rank::handle(const np_get_rank_response& aresponse)
	{
		int32_t lrank = 0;
		for (i64_actorid lroleid : aresponse.m_rolerank)
		{
			++lrank;
			int32_t lmailid = 0;
			int32_t lreward = 0;
			if (ttab_activity_toprank::instance().rankreward(m_tab->m_id, lrank, lmailid, lreward))
			{
				continue;
			}
			// Sendmail
			std::string lsrc = std::format("activity_rank activityid=[{}] role=[{}] mail=[{}] drop=[{}]", activityid(), nguid(lroleid), lmailid, lreward);
			if (!actor_activity_manage::get_drop().use(lreward, 1, lroleid, lsrc, nullptr, lmailid))
			{
				log_error()->print("activity_rank fail [{}]", lsrc);
			}
		}
		auto pro = std::make_shared<np_activityrank_operator>();
		pro->m_iscreate = false;
		pro->m_rankid = m_tab->m_id;
		actor::send_actor(actor_ranklist::actorid(), nguid::make(), pro);
		activity::finish();
	}
}//namespace ngl
