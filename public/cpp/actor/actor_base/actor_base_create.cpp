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
#include "actor_example_guess_number.h"
#include "actor_manage.h"
#include "actor_robot.h"
#include "actor_base.h"
#include "actor_role.h"
#include "actor_log.h"

namespace ngl
{
	// 用于创建非单例actor
	std::shared_ptr<actor_base> actor_base::create(ENUM_ACTOR atype, i16_area aarea, i32_actordataid aid, void* aparm/* = nullptr*/)
	{
		i64_actorid lactorid = nguid::make(atype, aarea, aid);
		if (actor_manage::instance().is_have_actor(lactorid))
		{
			return nullptr;
		}
		std::shared_ptr<actor_base> lpactor_base = nullptr;
		auto linitfun = []<typename TActorDerived, typename ...ARG>(ENUM_ACTOR actortype, std::shared_ptr<actor_base>& apactor, ARG&... arg)
		{
			std::make_shared<TActorDerived>(arg...)->template init_rfun<TActorDerived>();
			actor_base::first_nregister<TActorDerived>(actortype);
		};
		switch (atype)
		{
			case ACTOR_ROLE:
			{
				linitfun.operator()<actor_role>(ACTOR_ROLE, lpactor_base, tab_self_area, aid, aparm);
			}
			break;
			case ACTOR_ROBOT:
			{
				linitfun.operator()<actor_robot>(ACTOR_ROBOT, lpactor_base, tab_self_area, aid, aparm);
			}
			break;
			case ACTOR_LOG:
			{
				linitfun.operator()<actor_log>(ACTOR_LOG, lpactor_base, *(int32_t*)aparm);
			}
			break;
			case ACTOR_EXAMPLE_GUESS_NUMBER:
			{
				linitfun.operator()<actor_example_guess_number>(ACTOR_EXAMPLE_GUESS_NUMBER, lpactor_base, *(const std::map<int32_t, i64_actorid>*)aparm, aid);
			}
			break;
			default:
			{
				ngl::log_error()->print("actor_base::create({},{})", em<ENUM_ACTOR>::name(atype), aid);
				return nullptr;
			}
		}
		actor_manage::instance().add_actor(lpactor_base, [lpactor_base]() 
			{
				lpactor_base->set_activity_stat(actor_stat_free);
				lpactor_base->init();
				lpactor_base->init_db_component(false);
			}
		);
		return lpactor_base;
	}
}//namespace ngl