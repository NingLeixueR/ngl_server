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
// File overview: Implements logic for actor example manage.


#include "actor/actor_logic/actor_example_manage/actor_example_manage.h"
#include "actor/actor_logic/actor_example_match/actor_example_match.h"
#include "actor/actor_logic/actor_brief/actor_brief.h"

#include <list>

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
		return nguid::make(actor_type(), tab_self_area, nguid::none_actordataid());
	}

	void actor_example_manage::init()
	{
		// Bind the DB structure: DB.set(this);

		// Set up the timer_handle timer.
		np_timerparm tparm;
		if (make_timerparm::make_interval(tparm, 1) == false)
		{
			log_error()->print("actor_example_manage::init() make_timerparm::make_interval(tparm, 2) == false!!!");
			return;
		}
		set_timer(tparm);
	}

	void actor_example_manage::erase_actor_before()
	{
	}

	void actor_example_manage::loaddb_finish(pbdb::ENUM_DB atype, enum_dbstat astat)
	{
		if (atype != pbdb::ENUM_DB::ENUM_DB_ALL)
		{
			log_error()->print("actor_example_manage::loaddb_finish [{}:{}]", (int32_t)atype, (int32_t)astat);
			return;
		}
	}

	void actor_example_manage::enter_game(playinfo* applayinfo, i64_actorid aroleid, pbexample::ECROSS across, pbexample::EPLAY_TYPE atype)
	{
		log_error()->print("actor_example_manage enter_game role[{}]", aroleid);
		auto& lenter = applayinfo->m_role_enter_example;
		lenter.insert_or_assign(aroleid, true);
		{
			pbexample::PROBUFF_EXAMPLE_PLAY_ENTER_EXAMPLE_RESPONSE lresponse;
			lresponse.set_mcross(across);
			lresponse.set_mtype(atype);
			lresponse.set_mexampleactorid(applayinfo->m_actorexampleid);
			for (const auto& [lactorid, _] : lenter)
			{
				lresponse.add_mplayers(lactorid);
			}
			send_client(applayinfo->m_roles, lresponse);
		}
		if (lenter.size() >= applayinfo->m_roles.size())
		{
			i64_actorid lactorexampleid = applayinfo->m_actorexampleid;
			pbexample::EPLAY_TYPE ltype = applayinfo->m_type;
			{
				auto pro = std::make_shared<np_example_entergame_ready>();
				send_actor(lactorexampleid, id_guid(), pro);
			}
			{
				auto pro = std::make_shared<np_example_actorid>();
				pro->m_type = ltype;
				pro->m_actorexampleid = lactorexampleid;
				send_actor(applayinfo->m_roles, id_guid(), pro);
			}
			m_finishinfo[ltype][lactorexampleid] = *applayinfo;
			m_info[ltype].erase(lactorexampleid);
		}
	}
	
	void actor_example_manage::nregister()
	{
		// Timer.
		actor::register_timer<actor_example_manage>(&actor_example_manage::timer_handle);

		// Bind custom np_ messages.
		register_handle<actor_example_manage
			, np_create_example
			, np_example_equit
			, np_login_request_info
			, mforward<pbexample::PROBUFF_EXAMPLE_PLAY_ENTER_EXAMPLE>
		>(e_ready_all);
	}

	bool actor_example_manage::timer_handle([[maybe_unused]] const message<np_timerparm>& adata)
	{
		int32_t lnow = (int32_t)tools::time::gettime();
		pbexample::ECROSS lecross = tab_self_area > 0 ? pbexample::ECROSS::ECROSS_ORDINARY : pbexample::ECROSS::ECROSS_CROSS_ORDINARY;
		for (auto& [ltype, lplaymap] : m_info)
		{
			std::list<i64_actorid> lexampleids;
			for (const auto& lpair : lplaymap)
			{
				if (lpair.second.m_createexample + example_waittime <= lnow)
				{
					lexampleids.push_back(lpair.first);
				}
			}
			for (i64_actorid lexampleid : lexampleids)
			{
				auto lplay_it = lplaymap.find(lexampleid);
				if (lplay_it == lplaymap.end())
				{
					continue;
				}

				std::list<i64_actorid> lroleids;
				for (i64_actorid lroleid : lplay_it->second.m_roles)
				{
					if (lplay_it->second.m_role_enter_example.find(lroleid) == lplay_it->second.m_role_enter_example.end())
					{
						lroleids.push_back(lroleid);
					}
				}

				for (i64_actorid lroleid : lroleids)
				{
					auto lcur_it = lplaymap.find(lexampleid);
					if (lcur_it == lplaymap.end())
					{
						break;
					}
					enter_game(&lcur_it->second, lroleid, lecross, ltype);
				}
			}
		}
		return true;
	}
}//namespace ngl
