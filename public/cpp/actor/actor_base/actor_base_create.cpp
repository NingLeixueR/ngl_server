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
		if (actor_manage::getInstance().is_have_actor(lactorid))
		{
			return nullptr;
		}
		std::shared_ptr<actor_base> lpactor_base = nullptr;
		auto linitfun = [] <ENUM_ACTOR ACTORTYPE, typename T>(std::shared_ptr<actor_base>& apactor)
		{
			((T*)(apactor.get()))->init_rfun<T>();
			actor_base::first_nregister<T>(ACTORTYPE);
		};

		switch (atype)
		{
		case ACTOR_ROLE:
		{
			lpactor_base = std::make_shared<actor_role>(tab_self_area, aid, aparm);
			linitfun.operator()<ACTOR_ROLE, actor_role>(lpactor_base);
		}
		break;
		case ACTOR_ROBOT:
		{
			lpactor_base = std::make_shared<actor_robot>(tab_self_area, aid, aparm);
			linitfun.operator()<ACTOR_ROBOT, actor_robot>(lpactor_base);
		}
		break;
		case ACTOR_LOG:
		{
			lpactor_base = std::make_shared<actor_log>(aid);
			linitfun.operator()<ACTOR_LOG, actor_log>(lpactor_base);
		}
		break;
		case ACTOR_EXAMPLE_GUESS_NUMBER:
		{
			auto lroleids = (const std::map<int32_t, i64_actorid>*)aparm;
			lpactor_base = std::make_shared<actor_example_guess_number>(*lroleids, aid);
			linitfun.operator()<ACTOR_EXAMPLE_GUESS_NUMBER, actor_example_guess_number>(lpactor_base);
		}
		break;
		default:
			break;
		}
		if (lpactor_base == nullptr)
		{
			ngl::log_error()->print("actor_base::create({},{})", em<ENUM_ACTOR>::get_name(atype), aid);
			return nullptr;
		}
		actor_manage::getInstance().add_actor(lpactor_base, [lpactor_base]() 
			{
				lpactor_base->set_activity_stat(actor_stat_free);
				lpactor_base->init();
				lpactor_base->init_db_component(false);
			});
		return lpactor_base;
	}
}//namespace ngl