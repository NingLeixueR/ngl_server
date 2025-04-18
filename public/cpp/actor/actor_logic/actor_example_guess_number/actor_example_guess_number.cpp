#include "actor_example_guess_number.h"
#include "actor_brief.h"

namespace ngl
{
	actor_example_guess_number::actor_example_guess_number(const std::map<int32_t, i64_actorid>& aroleids, int32_t aindex) :
		actor_example<actor_example_guess_number>(pbexample::EPLAY_GUESS_NUMBER, ACTOR_EXAMPLE_GUESS_NUMBER, aindex, aroleids),
		m_index(0),
		m_maxnumber(101),
		m_minnumber(0),
		m_bombvalues(tools::rand() % 100 + 1),
		m_bombutc(0),
		m_bombrole(0)
	{
	}

	ENUM_ACTOR actor_example_guess_number::actor_type()
	{
		return ACTOR_EXAMPLE_GUESS_NUMBER;
	}

	i64_actorid actor_example_guess_number::actorid()
	{
		return nguid::make(actor_type(), tab_self_area, nguid::none_actordataid());
	}

	void actor_example_guess_number::init()
	{
		// 绑定DB结构:DB.set(this);

		// 设置timer_handle定时器
		np_timerparm tparm;
		if (make_timerparm::make_interval(tparm, 1) == false)
		{
			log_error()->print("actor_chat::init() make_timerparm::make_interval(tparm, 2) == false!!!");
			return;
		}
		set_timer(tparm);
	}

	void actor_example_guess_number::loaddb_finish(bool adbishave) 
	{
	}

	void actor_example_guess_number::next_setp()
	{
		auto pro = std::make_shared<pbexample::PROBUFF_EXAMPLE_GUESS_NUMBER_INFO>();
		pro->set_m_maxnumber(m_maxnumber);
		pro->set_m_minnumber(m_minnumber);
		pro->set_m_whoguess(m_rolesds[m_index % m_rolesds.size()]);
		++m_index;
		send_client(m_rolesds, pro);
		m_setputc = localtime::gettime();
	}

	void actor_example_guess_number::bomb(i64_actorid aroleid/* = nguid::make()*/)
	{
		auto pro = std::make_shared<pbexample::PROBUFF_EXAMPLE_GUESS_NUMBER_BOMB>();
		pro->set_m_bombvalue(m_bombvalues);
		pro->set_m_roleid(m_bombrole);
		if (aroleid == nguid::make())
		{
			send_client(m_rolesds, pro);
		}
		else
		{
			send_client(aroleid, pro);
		}
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
		return m_rolesds[m_index % m_rolesds.size()];
	}

	void actor_example_guess_number::send_error(i64_actorid aroleid, pbexample::E_GUESS_NUMBER_ERROR aerr)
	{
		auto pro = std::make_shared<pbexample::PROBUFF_EXAMPLE_GUESS_NUMBER_ERROR>();
		pro->set_m_stat(aerr);
		send_client(aroleid, pro);
	}

	void actor_example_guess_number::nregister()
	{
		// 定时器
		actor::register_timer<actor_example_guess_number>(&actor_example_guess_number::timer_handle);

		// 绑定自定义np_消息
		register_handle_custom<actor_example_guess_number>::func<
			np_example_entergame_ready
		>(true);

		// 绑定pb消息
		register_handle_proto<actor_example_guess_number>::func<
			mforward<pbexample::PROBUFF_EXAMPLE_GUESS_NUMBER>
		>(false);
	}


	bool actor_example_guess_number::timer_handle(const message<np_timerparm>& adata)
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
				lPROBUFF_EXAMPLE_GUESS_NUMBER.set_m_guessnumber(m_minnumber + 1);
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

	bool actor_example_guess_number::handle(const message<np_arg_null>&)
	{
		return true;
	}
}//namespace ngl