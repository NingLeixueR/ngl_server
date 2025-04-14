#include "actor_example_manage.h"
#include "actor_example_match.h"

namespace ngl
{
	actor_example_manage::actor_example_manage() :
		actor(
			actorparm
			{
				.m_parm
					{
						.m_type = ACTOR_EXAMPLE_MANAGE,
						.m_area = tab_self_area
					},
				.m_weight = 0x7fffffff,
			})
	{}

	ENUM_ACTOR actor_example_manage::actor_type()
	{
		return ACTOR_EXAMPLE_MANAGE;
	}

	i64_actorid actor_example_manage::actorid()
	{
		return nguid::make(ACTOR_EXAMPLE_MANAGE, tab_self_area, nguid::none_actordataid());
	}

	void actor_example_manage::loaddb_finish(bool adbishave) 
	{
	}

	void actor_example_manage::enter_game(playinfo* applayinfo, i64_actorid aroleid, pbexample::ECROSS across, pbexample::EPLAY_TYPE atype)
	{
		log_error()->print("actor_example_manage enter_game role[{}]", aroleid);
		applayinfo->m_role_enter_example[aroleid] = true;
		{
			auto lresponse = std::make_shared<pbexample::PROBUFF_EXAMPLE_PLAY_ENTER_EXAMPLE_RESPONSE>();
			lresponse->set_m_cross(across);
			lresponse->set_m_type(atype);
			lresponse->set_m_exampleactorid(applayinfo->m_actorexampleid);
			for (const auto& item : applayinfo->m_role_enter_example)
			{
				lresponse->add_m_players(item.first);
			}
			send_client(applayinfo->m_roles, lresponse);
		}
		if (applayinfo->m_role_enter_example.size() >= applayinfo->m_roles.size())
		{
			i64_actorid lactorexampleid = applayinfo->m_actorexampleid;
			pbexample::EPLAY_TYPE ltype = applayinfo->m_type;
			{
				auto pro = std::make_shared<np_example_entergame_ready>();
				send_actor(lactorexampleid, pro);
			}
			{
				auto pro = std::make_shared<np_example_actorid>();
				pro->m_type = ltype;
				pro->m_actorexampleid = lactorexampleid;
				actor::static_send_actor(lactorexampleid, id_guid(), pro);
			}
			m_finishinfo[ltype][lactorexampleid] = *applayinfo;
			m_info[ltype].erase(lactorexampleid);
		}
	}
		
	void actor_example_manage::init()
	{
		np_timerparm tparm;
		if (make_timerparm::make_interval(tparm, 1) == false)
		{
			log_error()->print("actor_chat::init() make_timerparm::make_interval(tparm, 2) == false!!!");
			return;
		}
		set_timer(tparm);
	}

	bool actor_example_manage::timer_handle(const message<np_timerparm>& adata)
	{
		int32_t lnow = localtime::gettime();
		pbexample::ECROSS lecross = tab_self_area > 0 ? pbexample::ECROSS::ECROSS_ORDINARY : pbexample::ECROSS::ECROSS_CROSS_ORDINARY;
		for (std::pair<const pbexample::EPLAY_TYPE, std::map<i64_actorid, playinfo>>& item1 : m_info)
		{
			for (std::pair<const i64_actorid, playinfo>& item2 : item1.second)
			{
				if (item2.second.m_createexample + example_waittime <= lnow)
				{
					for (i64_actorid roleid :item2.second.m_roles)
					{
						if (item2.second.m_role_enter_example.contains(roleid) == false)
						{
							enter_game(&item2.second, roleid, lecross, item1.first);
						}
					}
				}
			}
		}
		return true;
	}

	void actor_example_manage::nregister()
	{
		// 定时器
		actor::register_timer<actor_example_manage>(&actor_example_manage::timer_handle);

		// 协议注册
		register_handle_proto<actor_example_manage>::func<
			mforward<pbexample::PROBUFF_EXAMPLE_PLAY_ENTER_EXAMPLE>
		>(false);

		register_handle_custom<actor_example_manage>::func<
			np_create_example,
			np_example_equit
		>(true);
	}
}//namespace ngl