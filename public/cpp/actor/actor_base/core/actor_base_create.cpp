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
// File overview: Implements dynamic actor creation and registration.

#include "actor/actor_logic/actor_example_guess_number/actor_example_guess_number.h"
#include "actor/actor_logic/actor_testai/actor_testai.h"
#include "actor/actor_logic/actor_robot/actor_robot.h"
#include "actor/actor_logic/actor_role/actor_role.h"
#include "actor/actor_logic/actor_log/actor_log.h"
#include "actor/actor_base/core/actor_manage.h"
#include "actor/actor_base/core/actor_base.h"

namespace ngl
{
	// Factory entry used by generated registration code and one-off actor creation.
	std::shared_ptr<actor_base> actor_base::create(ENUM_ACTOR atype, i16_area aarea, i32_actordataid aid, void* aparm/* = nullptr*/)
	{
		i64_actorid lactorid = nguid::make(atype, aarea, aid);
		if (actor_manage::instance().is_have_actor(lactorid))
		{
			return nullptr;
		}
		auto linitfun = []<typename TActorDerived, typename ...ARG>(ENUM_ACTOR actortype, ARG&... arg)->std::shared_ptr<actor_base>
		{
			std::shared_ptr<actor_base> pactor = std::make_shared<TActorDerived>(arg...);
			// Wire the derived registration table before exposing the actor to the scheduler.
			std::static_pointer_cast<TActorDerived>(pactor)->template init_rfun<TActorDerived>();
			actor_base::first_nregister<TActorDerived>(actortype);
			actor_manage::instance().add_actor(pactor, [pactor]()
				{
					// Route sync completes before the actor becomes runnable and initializes DB state.
					pactor->set_activity_stat(actor_stat_free);
					pactor->init();
					pactor->init_db_component(false);
				}
			);
			return pactor;
		};
		switch (atype)
		{
			case ACTOR_ROLE:
				return linitfun.operator()<actor_role>(ACTOR_ROLE, tab_self_area, aid, aparm);
			case ACTOR_ROBOT:
				return linitfun.operator()<actor_robot>(ACTOR_ROBOT, tab_self_area, aid, aparm);
			case ACTOR_LOG:
				return linitfun.operator()<actor_log>(ACTOR_LOG, *(int32_t*)aparm);
			case ACTOR_EXAMPLE_GUESS_NUMBER:
				return linitfun.operator()<actor_example_guess_number>(ACTOR_EXAMPLE_GUESS_NUMBER, *(const std::map<int32_t, i64_actorid>*)aparm, aid);
			case ACTOR_TESTAI:
				return linitfun.operator()<actor_testai>(ACTOR_TESTAI, tab_self_area, aid);
			default:
			{
				ngl::log_error()->print("actor_base::create fail ({},{})", tools::em<ENUM_ACTOR>::name(atype), aid);
				return nullptr;
			}
		}
	}
}//namespace ngl
