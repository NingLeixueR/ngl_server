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
// File overview: Implements message handlers for message.
#include "actor/actor_logic/actor_example_manage/actor_example_manage.h"
#include "actor/actor_logic/actor_example_match/actor_example_match.h"
namespace ngl
{
	bool actor_example_manage::handle([[maybe_unused]] const message<mforward<pbexample::PROBUFF_EXAMPLE_PLAY_ENTER_EXAMPLE>>& adata)
	{
		const auto* lparm = adata.get_data();
		const pbexample::PROBUFF_EXAMPLE_PLAY_ENTER_EXAMPLE* lrecv = lparm->data();
		const i64_actorid lroleid = lparm->identifier();

		auto* ltypeinfo = tools::findmap(m_info, lrecv->mtype());
		if (ltypeinfo == nullptr)
		{
			actor_example_match::send_error(pbexample::EERROR_CODE_NOTFIND, nullptr, 0, lroleid);
			return true;
		}
		playinfo* lpplayinfo = tools::findmap(*ltypeinfo, lrecv->mexampleactorid());
		if (lpplayinfo == nullptr)
		{
			actor_example_match::send_error(pbexample::EERROR_CODE_NOTFIND, nullptr, 0, lroleid);
			return true;
		}

		if (!lpplayinfo->m_roles.contains(lroleid))
		{
			actor_example_match::send_error(pbexample::EERROR_CODE_NOTFIND, nullptr, 0, lroleid);
			return true;
		}

		enter_game(lpplayinfo, lroleid, lrecv->mcross(), lrecv->mtype());

		return true;
	}
	bool actor_example_manage::handle([[maybe_unused]] const message<np_create_example>& adata)
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
			lplayinfo.m_createexample = (int32_t)tools::localtime::gettime();
			lplayinfo.m_actorexampleid = lpactor->id_guid();
			for (const auto& [_, lactorid] : lprecv->m_roleids)
			{
				lplayinfo.m_roles.insert(lactorid);
				std::get<0>(m_playerexample[lactorid]) = lprecv->m_type;
				std::get<1>(m_playerexample[lactorid]) = lpactor->id_guid();
			}

			pbexample::PROBUFF_EXAMPLE_PLAY_CREATE pro;
			pro.set_mexampleactorid(lpactor->id_guid());
			pro.set_mtype(lprecv->m_type);
			pro.set_mstat(pbexample::PROBUFF_EXAMPLE_PLAY_CREATE_estat_estat_success);
			send_client(lplayinfo.m_roles, pro);
		}
		else
		{
			for (const auto& [_, lactorid] : lprecv->m_roleids)
			{
				actor_example_match::send_error(pbexample::EERROR_CODE_CREATEGAME_FAIL, nullptr, 0, lactorid);
			}
		}
		return true;
	}
	bool actor_example_manage::handle([[maybe_unused]] const message<np_example_equit>& adata)
	{
		const np_example_equit* lprecv = adata.get_data();
		auto lpdata = tools::findmap(m_finishinfo, lprecv->m_type);
		if (lpdata == nullptr)
		{
			return true;
		}
		auto lpexample = tools::findmap(*lpdata, lprecv->m_exampleid);
		if (lpexample == nullptr)
		{
			return true;
		}
		for (const i64_actorid lplayer : lpexample->m_roles)
		{
			m_playerexample.erase(lplayer);
		}

		auto pro = std::make_shared<np_example_actorid>();
		pro->m_type = lprecv->m_type;
		pro->m_actorexampleid = 0;
		actor::send_actor(lpexample->m_roles, id_guid(), pro);

		lpdata->erase(lprecv->m_exampleid);
		return true;
	}
	bool actor_example_manage::handle([[maybe_unused]] const message<np_login_request_info>& adata)
	{
		const np_login_request_info* lprecv = adata.get_data();
		type_tuple* lptuple = tools::findmap(m_playerexample, lprecv->m_roleid);
		if (lptuple == nullptr)
		{
			return true;
		}

		auto pro = std::make_shared<np_example_actorid>();
		pro->m_type = std::get<0>(*lptuple);
		pro->m_actorexampleid = std::get<1>(*lptuple);
		actor::send_actor(lprecv->m_roleid, id_guid(), pro);
		return true;
	}
}//namespace ngl
