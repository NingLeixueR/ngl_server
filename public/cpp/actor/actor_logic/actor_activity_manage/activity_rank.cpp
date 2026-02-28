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
		// # 发送排行奖励 
		// ## 1、向rank模块发送消息，拉去玩家排行信息
		// ## 2、发送奖励
		auto pro = std::make_shared<np_get_rank>();
		pro->m_rankid = m_tab->m_id;

		actor::send_actor(actor_ranklist::actorid(), actor_activity_manage::actorid(), pro);
		return;
	}

	void activity_rank::handle(const np_get_rank_response& aresponse)
	{
		for (int i = 0;i< aresponse.m_rolerank.size();++i)
		{
			int32_t lmailid = 0;
			int32_t lreward = 0;
			if (ttab_activity_toprank::instance().rankreward(m_tab->m_id, i + 1, lmailid, lreward))
			{
				continue;
			}
			// 发送邮件
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