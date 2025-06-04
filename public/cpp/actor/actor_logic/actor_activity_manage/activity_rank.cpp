#include "actor_activity_manage.h"
#include "activity_rank.h"
#include "actor_ranklist.h"


namespace ngl
{
	void activity_rank::start()
	{
		auto pro = std::make_shared<np_activityrank_operator>();
		pro->m_iscreate = true;
		pro->m_rankid = activityid();
		actor::static_send_actor(actor_ranklist::actorid(), nguid::make(), pro);
		activity::start();
	}

	void activity_rank::finish()
	{
		// # 发送排行奖励 
		// ## 1、向rank模块发送消息，拉去玩家排行信息
		// ## 2、发送奖励
		auto pro = std::make_shared<np_get_rank>();
		pro->m_rankid = activityid();

		actor::static_send_actor(actor_ranklist::actorid(), actor_activity_manage::actorid(), pro);
		return;
	}

	void activity_rank::handle(const np_get_rank_response& aresponse)
	{
		for (int i = 0;i< aresponse.m_rolerank.size();++i)
		{
			int32_t lmailid = 0;
			int32_t lreward = 0;
			if (ttab_activity_toprank::rankreward(activityid(), i + 1, lmailid, lreward))
			{
				continue;
			}
			// 发送邮件
			std::string lsrc = std::format(
				"activity_rank activityid=[{}] role=[{}] mail=[{}] drop=[{}]",
				activityid(), nguid(aresponse.m_rolerank[i]), lmailid, lreward
			);
			if (!actor_drop::use(lreward, 1, aresponse.m_rolerank[i], lsrc, nullptr, lmailid))
			{
				log_error()->print("activity_rank fail [{}]", lsrc);
			}
		}
		auto pro = std::make_shared<np_activityrank_operator>();
		pro->m_iscreate = false;
		pro->m_rankid = activityid();
		actor::static_send_actor(actor_ranklist::actorid(), nguid::make(), pro);
		activity::finish();
	}

}//namespace ngl