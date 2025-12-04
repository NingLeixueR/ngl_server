/*
* Copyright (c) [2020-2025] NingLeixueR
* 
* 项目名称：ngl_server
* 项目地址：https://github.com/NingLeixueR/ngl_server
* 
* 本文件是 ngl_server 项目的一部分，遵循 MIT 开源协议发布。
* 您可以按照协议规定自由使用、修改和分发本项目，包括商业用途，
* 但需保留原始版权和许可声明。
* 
* 许可详情参见项目根目录下的 LICENSE 文件：
* https://github.com/NingLeixueR/ngl_server/blob/main/LICENSE
*/
#include "actor_example_guess_number.h"
#include "actor_brief.h"

namespace ngl
{
	actor_example_guess_number::actor_example_guess_number(const std::map<int32_t, i64_actorid>& aroleids, int32_t aindex) :
		actor_example<actor_example_guess_number>(pbexample::EPLAY_GUESS_NUMBER, ACTOR_EXAMPLE_GUESS_NUMBER, aindex, aroleids),
		m_index(0),
		m_maxnumber(guess_number_max_number),
		m_minnumber(guess_number_min_number),
		m_bombvalues(tools::rand() % guess_number_max_number + 1),
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
		actor_example::init();

		// 绑定DB结构:DB.set(this);

		// 设置timer_handle定时器
		np_timerparm tparm;
		if (make_timerparm::make_interval(tparm, 1) == false)
		{
			log_error()->print("actor_example_guess_number::init() make_timerparm::make_interval(tparm, 2) == false!!!");
			return;
		}
		set_timer(tparm);
	}

	void actor_example_guess_number::loaddb_finish(pbdb::ENUM_DB atype, enum_dbstat astat)
	{
		if (atype != pbdb::ENUM_DB::ENUM_DB_ALL)
		{
			log_error()->print("actor_example_guess_number::loaddb_finish [{}:{}]", (int32_t)atype, (int32_t)astat);
			return;
		}
	}

	void actor_example_guess_number::next_setp()
	{
		auto pro = std::make_shared<pbexample::PROBUFF_EXAMPLE_GUESS_NUMBER_INFO>();
		pro->set_mmaxnumber(m_maxnumber);
		pro->set_mminnumber(m_minnumber);
		pro->set_mwhoguess(m_rolesds[m_index % m_rolesds.size()]);
		++m_index;
		send_client(m_rolesds, pro);
		m_setputc = (int32_t)localtime::gettime();
	}

	void actor_example_guess_number::bomb(i64_actorid aroleid/* = nguid::make()*/)
	{
		auto pro = std::make_shared<pbexample::PROBUFF_EXAMPLE_GUESS_NUMBER_BOMB>();
		pro->set_mbombvalue(m_bombvalues);
		pro->set_mroleid(m_bombrole);
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
		m_bombutc = (int32_t)localtime::gettime();
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
		pro->set_mstat(aerr);
		send_client(aroleid, pro);
	}

	void actor_example_guess_number::nregister()
	{
		// 定时器
		actor::register_timer<actor_example_guess_number>(&actor_example_guess_number::timer_handle);

		// 绑定自定义np_与pb消息
		register_handle<actor_example_guess_number>::func<
			np_example_entergame_ready
			, mforward<pbexample::PROBUFF_EXAMPLE_GUESS_NUMBER>
		>(nready::e_ready_all);
	}


	bool actor_example_guess_number::timer_handle(const message<np_timerparm>& adata)
	{
		int32_t lnow = (int32_t)localtime::gettime();
		if (isfinish() == false)
		{
			if (m_setputc == 0)
			{
				return true;
			}
			// 检查当前阶段时间
			if (m_setputc + guess_number_esetp_maxtime <= lnow)
			{
				i64_actorid roleid = next_guess_role();
				pbexample::PROBUFF_EXAMPLE_GUESS_NUMBER lPROBUFF_EXAMPLE_GUESS_NUMBER;
				lPROBUFF_EXAMPLE_GUESS_NUMBER.set_mguessnumber(m_minnumber + 1);
				mforward<pbexample::PROBUFF_EXAMPLE_GUESS_NUMBER> pro(roleid, lPROBUFF_EXAMPLE_GUESS_NUMBER);
				message<mforward<pbexample::PROBUFF_EXAMPLE_GUESS_NUMBER>> lmessage(0, nullptr, &pro);
				handle(lmessage);
			}
		}
		else
		{
			if (m_bombutc + guess_number_equit_time <= lnow)
			{
				erase_actor();
			}
		}
		return true;
	}

	bool actor_example_guess_number::handle(const message<np_arg_null>&)
	{
		return true;
	}
}//namespace ngl