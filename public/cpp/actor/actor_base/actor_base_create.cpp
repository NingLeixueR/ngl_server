#include "actor_ugpalace.h"
#include "actor_manage.h"
#include "actor_robot.h"
#include "actor_base.h"
#include "actor_role.h"
#include "actor_log.h"

namespace ngl
{
	// 用于创建非单例actor
	actor_base* actor_base::create(ENUM_ACTOR atype, i32_actordataid aid/*, actor_base* aactor*/, void* aparm/* = nullptr*/)
	{
		if (actor_manage::getInstance().is_have_actor(nguid::make(atype, tab_self_area, aid)))
			return nullptr;
		actor_base* lpactor_base = nullptr;
		switch (atype)
		{
		case ACTOR_ROLE:
			lpactor_base = new actor_role(tab_self_area, aid, aparm);
			((actor_role*)(lpactor_base))->init_rfun<actor_role>();
			actor_base::first_nregister<actor_role>();
			break;
		case ACTOR_ROBOT:
			lpactor_base = new actor_robot(tab_self_area, aid, aparm);
			((actor_robot*)(lpactor_base))->init_rfun<actor_robot>();
			actor_base::first_nregister<actor_robot>();
			break;
		case ACTOR_PLAYS_GO_UNDERGROUNDPALACE:
			lpactor_base = new actor_ugpalace(atype, aid, aparm);
			((actor_ugpalace*)(lpactor_base))->init_rfun<actor_ugpalace>();
			actor_base::first_nregister<actor_ugpalace>();
			break;
		//case ACTOR_ACTIVITY:
		//	lpactor_base = activity_create::create(aid, *(bool*)aparm);
		//	break;
		case ACTOR_LOG:
			lpactor_base = new actor_log(*(ELOG_TYPE*)aparm);
			((actor_log*)(lpactor_base))->init_rfun<actor_log>();
			actor_base::first_nregister<actor_log>();
			break;
		default:
			break;
		}
		if (lpactor_base == nullptr)
		{
			LogLocalError("actor_base::create(%,%)", (int)atype, aid)
			return nullptr;
		}
		actor_manage::getInstance().add_actor(lpactor_base, [lpactor_base]() 
			{
				Try
				{
					lpactor_base->set_activity_stat(actor_stat_free);
					lpactor_base->init();
					lpactor_base->init_db_component(false);
				}
				Catch
			});
		return lpactor_base;
	}
}//namespace ngl