#include "actor_example_guess_number.h"
#include "actor_brief.h"

namespace ngl
{
	actor_example_guess_number::actor_example_guess_number(const std::set<i64_actorid>& aroleids, int32_t aindex) :
		actor(
			actorparm
			{
				.m_parm
					{
						.m_type = ACTOR_EXAMPLE_GUESS_NUMBER,
						.m_area = tab_self_area,
						.m_id	= aindex
					},
				.m_weight = 0x7fffffff,
			}),
		m_index(0),
		m_maxnumber(101),
		m_minnumber(0),
		m_bombvalues(tools::rand() % 100 + 1)
	{
		m_rolesds.assign(aroleids.begin(), aroleids.end());
		tdb_brief::nsp_cli<actor_example_guess_number>::init(actor_brief::actor_type(), this, aroleids);
	}

	void actor_example_guess_number::nregister()
	{
		// 协议注册
		register_handle_proto<actor_example_guess_number>::func<
			mforward<pbexample::PROBUFF_EXAMPLE_GUESS_NUMBER>
		>(false);

		register_handle_custom<actor_example_guess_number>::func<
			np_example_entergame_ready
		>(true);
	}

	void actor_example_guess_number::next_setp()
	{
		auto pro = std::make_shared<pbexample::PROBUFF_EXAMPLE_GUESS_NUMBER_INFO>();
		pro->set_m_maxnumber(m_maxnumber);
		pro->set_m_minnumber(m_minnumber);
		pro->set_m_whoguess(m_rolesds[m_index% m_rolesds.size()]);
		++m_index;
		send_client(m_rolesds, pro);
		m_setputc = localtime::gettime();
	}

	bool actor_example_guess_number::handle(const message<np_example_entergame_ready>& adata)
	{
		next_setp();
		return true;
	}

	void actor_example_guess_number::bomb(i64_actorid aroleid)
	{
		auto pro = std::make_shared<pbexample::PROBUFF_EXAMPLE_GUESS_NUMBER_BOMB>();
		pro->set_m_bombvalue(m_bombvalues);
		pro->set_m_roleid(aroleid);
		send_client(m_rolesds, pro);
	}

	bool actor_example_guess_number::handle(const message<mforward<pbexample::PROBUFF_EXAMPLE_GUESS_NUMBER>>& adata)
	{
		const pbexample::PROBUFF_EXAMPLE_GUESS_NUMBER* lpdata = adata.get_data()->data();
		i64_actorid roleid = adata.get_data()->identifier();

		int32_t lnumber = lpdata->m_guessnumber();
		if (lnumber >= m_maxnumber || lnumber <= m_minnumber)
		{
			auto pro = std::make_shared<pbexample::PROBUFF_EXAMPLE_GUESS_NUMBER_ERROR>();
			pro->set_m_stat(pbexample::E_GUESS_NUMBER_ERROR_VALUE);
			send_client(roleid, pro);
			return true;
		}

		if (lnumber == m_bombvalues)
		{//炸了 游戏结束
			bomb(roleid);
			return true;
		}

		if (lnumber > m_bombvalues)
		{
			m_maxnumber = lnumber;
		}
		else
		{
			m_minnumber = lnumber;
		}

		int32_t lsize = m_maxnumber - 1 - m_minnumber;
		if (lsize <= 1)
		{//游戏结束 下一个人被炸
			bomb(m_rolesds[m_index % m_rolesds.size()]);
			return true;
		}

		next_setp();
		return true;
	}

	bool actor_example_guess_number::timer_handle(const message<timerparm>& adata)
	{
		return true;
	}

}//namespace ngl