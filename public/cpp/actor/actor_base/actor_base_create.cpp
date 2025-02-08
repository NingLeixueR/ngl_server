#include "actor_ugpalace.h"
#include "actor_manage.h"
#include "actor_robot.h"
#include "actor_base.h"
#include "actor_role.h"
#include "actor_log.h"

namespace ngl
{

	// 用于创建非单例actor
	actor_base* actor_base::create(ENUM_ACTOR atype, i16_area aarea, i32_actordataid aid, void* aparm/* = nullptr*/)
	{
		i64_actorid lactorid = nguid::make(atype, aarea, aid);
		if (actor_manage::getInstance().is_have_actor(lactorid))
		{
			return nullptr;
		}
		actor_base* lpactor_base = nullptr;
		switch (atype)
		{
		case ACTOR_ROLE:
		{
			lpactor_base = new actor_role(tab_self_area, aid, aparm);
			((actor_role*)(lpactor_base))->init_rfun<actor_role>();
			actor_base::first_nregister<actor_role>(ACTOR_ROLE);
		}
		break;
		case ACTOR_ROBOT:
		{
			lpactor_base = new actor_robot(tab_self_area, aid, aparm);
			((actor_robot*)(lpactor_base))->init_rfun<actor_robot>();
			actor_base::first_nregister<actor_robot>(ACTOR_ROBOT);
		}
		break;
		case ACTOR_PLAYS_GO_UNDERGROUNDPALACE:
		{
			lpactor_base = new actor_ugpalace(atype, aid, aparm);
			((actor_ugpalace*)(lpactor_base))->init_rfun<actor_ugpalace>();
			actor_base::first_nregister<actor_ugpalace>(ACTOR_PLAYS_GO_UNDERGROUNDPALACE);
		}
		break;
		case ACTOR_LOG:
		{
			lpactor_base = new actor_log(aid);
			((actor_log*)(lpactor_base))->init_rfun<actor_log>();
			actor_base::first_nregister<actor_log>(ACTOR_LOG);
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