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
// File overview: Implements logic for actor activity manage.


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
		// # Sendrankreward
		// ## 1, Torankmodulesendmessage, playerrankinfo
		// ## 2, Sendreward
		auto pro = std::make_shared<np_get_rank>();
		pro->m_rankid = m_tab->m_id;

		actor::send_actor(actor_ranklist::actorid(), actor_activity_manage::actorid(), pro);
		return;
	}

	void activity_rank::handle(const np_get_rank_response& aresponse)
	{
		for (std::size_t i = 0; i < aresponse.m_rolerank.size(); ++i)
		{
			int32_t lmailid = 0;
			int32_t lreward = 0;
			if (ttab_activity_toprank::instance().rankreward(m_tab->m_id, static_cast<int32_t>(i + 1), lmailid, lreward))
			{
				continue;
			}
			// Sendmail
			std::string lsrc = std::format("activity_rank activityid=[{}] role=[{}] mail=[{}] drop=[{}]", activityid(), nguid(aresponse.m_rolerank[i]), lmailid, lreward);
			if (!actor_activity_manage::get_drop().use(lreward, 1, aresponse.m_rolerank[i], lsrc, nullptr, lmailid))
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