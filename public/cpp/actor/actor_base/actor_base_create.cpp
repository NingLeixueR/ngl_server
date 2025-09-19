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
		auto linitfun = []<typename TActorDerived, typename ...ARG>(
			ENUM_ACTOR actortype, std::shared_ptr<actor_base>& apactor, ARG&... arg
			)
		{
			apactor = std::make_shared<TActorDerived>(arg...);
			((TActorDerived*)(apactor.get()))->template init_rfun<TActorDerived>();
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
				linitfun.operator()<actor_example_guess_number>(
					ACTOR_EXAMPLE_GUESS_NUMBER, lpactor_base, *(const std::map<int32_t, i64_actorid>*)aparm, aid
				);
			}
			break;
			default:
			{
				ngl::log_error()->print("actor_base::create({},{})", em<ENUM_ACTOR>::get_name(atype), aid);
				return nullptr;
			}
		}
		actor_manage::instance().add_actor(lpactor_base, [lpactor_base]() 
			{
				lpactor_base->set_activity_stat(actor_stat_free);
				lpactor_base->init();
				lpactor_base->init_db_component(false);
			});
		return lpactor_base;
	}
}//namespace ngl