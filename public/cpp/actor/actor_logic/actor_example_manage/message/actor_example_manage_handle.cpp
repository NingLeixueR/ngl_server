#include "actor_example_manage.h"
#include "actor_example_match.h"

namespace ngl
{
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
		for (i64_actorid player : lpexample->m_roles)
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

		return true;
	}
}//namespace ngl