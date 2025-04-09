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
		
	bool actor_example_manage::handle(const message<np_create_example>& adata)
	{
		const np_create_example* lprecv = adata.get_data();
		if (lprecv == nullptr)
		{
			return true;
		}
		std::shared_ptr<actor_base> lpactor = nullptr;
		
		switch (lprecv->m_type)
		{
		case pbexample::EPLAY_GUESS_NUMBER:
		{
			lpactor = actor_base::create(ACTOR_EXAMPLE_GUESS_NUMBER, tab_self_area, ++m_exampleindex[pbexample::EPLAY_GUESS_NUMBER], (void*)&lprecv->m_roleids);
		}
		break;
		}

		if (lpactor != nullptr)
		{
			playinfo& lplayinfo = m_info[lprecv->m_type][lpactor->id_guid()];
			lplayinfo.m_createexample = localtime::gettime();
			lplayinfo.m_actorexampleid = lpactor->id_guid();
			for (const auto& lpair : lprecv->m_roleids)
			{
				lplayinfo.m_roles.insert(lpair.second);
				std::get<0>(m_playerexample[lpair.second]) = lprecv->m_type;
				std::get<1>(m_playerexample[lpair.second]) = lpactor->id_guid();
			}		

			auto pro = std::make_shared<pbexample::PROBUFF_EXAMPLE_PLAY_CREATE>();
			pro->set_m_exampleactorid(lpactor->id_guid());
			pro->set_m_type(lprecv->m_type);
			pro->set_m_stat(pbexample::PROBUFF_EXAMPLE_PLAY_CREATE_estat_estat_success);
			send_client(lplayinfo.m_roles, pro);
		}	
		else
		{
			for (const auto& lpair : lprecv->m_roleids)
			{
				actor_example_match::send_error(pbexample::EERROR_CODE_CREATEGAME_FAIL, nullptr, 0, lpair.second);
			}
		}
		return true;
	}

	void actor_example_manage::enter_game(playinfo* applayinfo, i64_actorid aroleid, pbexample::ECROSS across, pbexample::EPLAY_TYPE atype)
	{
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
	}

	bool actor_example_manage::handle(const message<mforward<pbexample::PROBUFF_EXAMPLE_PLAY_ENTER_EXAMPLE>>& adata)
	{
		const pbexample::PROBUFF_EXAMPLE_PLAY_ENTER_EXAMPLE* lpdata = adata.get_data()->data();
		i64_actorid roleid = adata.get_data()->identifier();

		playinfo* lpplayinfo = tools::findmap(m_info[lpdata->m_type()], lpdata->m_exampleactorid());
		if (lpplayinfo == nullptr)
		{
			actor_example_match::send_error(pbexample::EERROR_CODE_NOTFIND, nullptr, 0, roleid);
			return true;
		}

		if (lpplayinfo->m_roles.contains(roleid) == false)
		{
			actor_example_match::send_error(pbexample::EERROR_CODE_NOTFIND, nullptr, 0, roleid);
			return true;
		}

		enter_game(lpplayinfo, roleid, lpdata->m_cross(), lpdata->m_type());

		if (lpplayinfo->m_role_enter_example.size() >= lpplayinfo->m_roles.size())
		{
			{
				auto pro = std::make_shared<np_example_entergame_ready>();
				send_actor(lpdata->m_exampleactorid(), pro);
			}
			{
				auto pro = std::make_shared<np_example_actorid>();
				pro->m_type = lpdata->m_type();
				pro->m_actorexampleid = lpdata->m_exampleactorid();
				actor::static_send_actor(lpplayinfo->m_roles, id_guid(), pro);
			}
			m_finishinfo[lpdata->m_type()][lpdata->m_exampleactorid()] = *lpplayinfo;
			m_info[lpdata->m_type()].erase(lpdata->m_exampleactorid());
		}
		return true;
	}

	bool actor_example_manage::handle(const message<np_login_request_info>& adata)
	{
		type_tuple* lptuple = tools::findmap(m_playerexample, adata.get_data()->m_roleid);
		if (lptuple == nullptr)
		{
			return true;
		}

		auto pro = std::make_shared<np_example_actorid>();
		pro->m_type = std::get<0>(*lptuple);
		pro->m_actorexampleid = std::get<1>(*lptuple);
		send_actor(adata.get_data()->m_roleid, pro);
		
		return true;
	}

	bool actor_example_manage::handle(const message<np_example_equit>& adata)
	{
		const np_example_equit* lprecv = adata.get_data();
		auto lpdata = tools::findmap(m_finishinfo, adata.get_data()->m_type);
		if (lpdata == nullptr)
		{
			return true;
		}
		auto lpexample = tools::findmap(*lpdata, lprecv->m_exampleid);
		if (lpexample == nullptr)
		{
			return true;
		}
		for (i64_actorid player: lpexample->m_roles)
		{
			m_playerexample.erase(player);
		}

		auto pro = std::make_shared<np_example_actorid>();
		pro->m_type = adata.get_data()->m_type;
		pro->m_actorexampleid = 0;
		actor::static_send_actor(lpexample->m_roles, id_guid(), pro);

		lpdata->erase(lprecv->m_exampleid);
		return true;
	}

	void actor_example_manage::init()
	{
		timerparm tparm;
		if (make_timerparm::make_interval(tparm, 1) == false)
		{
			log_error()->print("actor_chat::init() make_timerparm::make_interval(tparm, 2) == false!!!");
			return;
		}
		set_timer(tparm);
	}

	bool actor_example_manage::timer_handle(const message<timerparm>& adata)
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
							log_error()->print("actor_example_manage::timer_handle enter_game role[{}]", roleid);
							enter_game(&item2.second, roleid, lecross, item1.first);
						}
					}
				}
			}
		}
		return true;
	}

}//namespace ngl