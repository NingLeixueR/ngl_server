#include "actor_example_manage.h"



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
		bool lcreate = false;
		actor_base* lpactor = nullptr;
		switch (lprecv->m_type)
		{
		case pbexample::EPLAY_GUESS_NUMBER:
		{
			lpactor = actor_base::create(
				ACTOR_EXAMPLE_GUESS_NUMBER, 
				tab_self_area, 
				++m_exampleindex[pbexample::EPLAY_GUESS_NUMBER],
				(void*)&lprecv->m_roleids
			);
			lcreate = lpactor != nullptr;
		}
		break;
		}
		if (lcreate)
		{
			m_info[lprecv->m_type][lpactor->id_guid()].m_roles = lprecv->m_roleids;
			m_info[lprecv->m_type][lpactor->id_guid()].m_createexample = localtime::gettime();
			for (int64_t roleid : lprecv->m_roleids)
			{
				std::get<0>(m_playerexample[roleid]) = lprecv->m_type;
				std::get<1>(m_playerexample[roleid]) = lpactor->id_guid();
			}		


			auto pro = std::make_shared<pbexample::PROBUFF_EXAMPLE_PLAY_CREATE>();
			pro->set_m_exampleactorid(lpactor->id_guid());
			pro->set_m_type(lprecv->m_type);
			pro->set_m_stat(pbexample::PROBUFF_EXAMPLE_PLAY_CREATE_estat_estat_success);
			send_client(lprecv->m_roleids, pro);
		}
		
		return true;
	}
	
	bool actor_example_manage::handle(const message<mforward<pbexample::PROBUFF_EXAMPLE_PLAY_ENTER_EXAMPLE>>& adata)
	{
		const pbexample::PROBUFF_EXAMPLE_PLAY_ENTER_EXAMPLE* lpdata = adata.get_data()->data();
		i64_actorid roleid = adata.get_data()->identifier();

		playinfo* lpplayinfo = tools::findmap(m_info[lpdata->m_type()], lpdata->m_exampleactorid());
		if (lpplayinfo == nullptr)
		{
			return true;
		}
		if (lpplayinfo->m_roles.contains(roleid) == false)
		{
			return true;
		}
		lpplayinfo->m_role_enter_example[roleid] = true;
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

}//namespace ngl