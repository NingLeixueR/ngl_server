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
		m_bombvalues(tools::rand() % 100 + 1),
		m_bombutc(0),
		m_bombrole(0)
	{
		m_rolesds.assign(aroleids.begin(), aroleids.end());
		tdb_brief::nsp_cli<actor_example_guess_number>::init(actor_brief::actor_type(), this, aroleids);
	}

	void actor_example_guess_number::nregister()
	{
		// 定时器
		actor::register_timer<actor_example_guess_number>(&actor_example_guess_number::timer_handle);

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

	void actor_example_guess_number::bomb()
	{
		auto pro = std::make_shared<pbexample::PROBUFF_EXAMPLE_GUESS_NUMBER_BOMB>();
		pro->set_m_bombvalue(m_bombvalues);
		pro->set_m_roleid(m_bombrole);
		send_client(m_rolesds, pro);
	}

	bool actor_example_guess_number::isfinish()
	{
		return m_bombutc != 0 || m_bombrole != 0;
	}

	void actor_example_guess_number::set_finish(i64_actorid abombrole)
	{
		if (isfinish())
		{
			return;
		}
		m_bombutc = localtime::gettime();
		m_bombrole = abombrole;
	}

	i64_actorid actor_example_guess_number::next_guess_role()
	{
		if (isfinish())
		{
			return nguid::make();
		}
		return m_rolesds[m_index% m_rolesds.size()];
	}

	void actor_example_guess_number::init()
	{
		timerparm tparm;
		if (make_timerparm::make_interval(tparm, 1) == false)
		{
			log_error()->print("actor_chat::init() make_timerparm::make_interval(tparm, 2) == false!!!");
			return;
		}
		set_timer(tparm);
	}

	bool actor_example_guess_number::handle(const message<mforward<pbexample::PROBUFF_EXAMPLE_GUESS_NUMBER>>& adata)
	{
		const pbexample::PROBUFF_EXAMPLE_GUESS_NUMBER* lpdata = adata.get_data()->data();
		i64_actorid roleid = adata.get_data()->identifier();

		if (isfinish())
		{//游戏已经结束
			bomb();
			return true;
		}
		if (roleid != next_guess_role())
		{
			return true;
		}
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
			set_finish(roleid);
			bomb();
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
			set_finish(m_rolesds[next_guess_role()]);
			bomb();
			return true;
		}

		next_setp();
		return true;
	}

	bool actor_example_guess_number::timer_handle(const message<timerparm>& adata)
	{
		int32_t lnow = localtime::gettime();
		if (isfinish() == false)
		{
			if (m_setputc == 0)
			{
				return true;
			}
			// 检查当前阶段时间
			if (m_setputc + esetp_maxtime <= lnow)
			{
				i64_actorid roleid = next_guess_role();
				pbexample::PROBUFF_EXAMPLE_GUESS_NUMBER lPROBUFF_EXAMPLE_GUESS_NUMBER;
				lPROBUFF_EXAMPLE_GUESS_NUMBER.set_m_guessnumber(m_minnumber+1);
				mforward<pbexample::PROBUFF_EXAMPLE_GUESS_NUMBER> pro(roleid, lPROBUFF_EXAMPLE_GUESS_NUMBER);
				message<mforward<pbexample::PROBUFF_EXAMPLE_GUESS_NUMBER>> lmessage(0, nullptr, &pro);
				handle(lmessage);
			}
		}
		else
		{
			if (m_bombutc + equit_time <= lnow)
			{
				erase_actor_byid();
			}
		}
		return true;
	}

}//namespace ngl