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
// File overview: Implements guess-number game actor construction and turn logic.


#include "actor/actor_logic/actor_example_guess_number/actor_example_guess_number.h"
#include "actor/actor_logic/actor_brief/actor_brief.h"

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

		// Bind the DB structure: DB.set(this);

		// Set up the timer_handle timer.
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
		pbexample::PROBUFF_EXAMPLE_GUESS_NUMBER_INFO pro;
		pro.set_mmaxnumber(m_maxnumber);
		pro.set_mminnumber(m_minnumber);
		pro.set_mwhoguess(m_rolesds[m_index % m_rolesds.size()]);
		++m_index;
		send_client(m_rolesds, pro);
		m_setputc = (int32_t)tools::time::gettime();
	}

	void actor_example_guess_number::bomb(i64_actorid aroleid/* = nguid::make()*/)
	{
		pbexample::PROBUFF_EXAMPLE_GUESS_NUMBER_BOMB pro;
		pro.set_mbombvalue(m_bombvalues);
		pro.set_mroleid(m_bombrole);
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
		m_bombutc = (int32_t)tools::time::gettime();
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

	i64_actorid actor_example_guess_number::cur_guess_role()
	{
		if (isfinish() || m_rolesds.empty())
		{
			return nguid::make();
		}
		int32_t lindex = m_index == 0 ? 0 : m_index - 1;
		return m_rolesds[lindex % m_rolesds.size()];
	}

	void actor_example_guess_number::send_error(i64_actorid aroleid, pbexample::E_GUESS_NUMBER_ERROR aerr)
	{
		pbexample::PROBUFF_EXAMPLE_GUESS_NUMBER_ERROR pro;
		pro.set_mstat(aerr);
		send_client(aroleid, pro);
	}

	void actor_example_guess_number::nregister()
	{
		// Timer.
		actor::register_timer<actor_example_guess_number>(&actor_example_guess_number::timer_handle);

		// Bind np_andpbmessage
		register_handle<actor_example_guess_number
			, np_example_entergame_ready
			, mforward<pbexample::PROBUFF_EXAMPLE_GUESS_NUMBER>
		>();
	}


	bool actor_example_guess_number::timer_handle([[maybe_unused]] const message<np_timerparm>& adata)
	{
		int32_t lnow = (int32_t)tools::time::gettime();
		if (isfinish() == false)
		{
			if (m_setputc == 0)
			{
				return true;
			}
			// Current time
			if (m_setputc + guess_number_esetp_maxtime <= lnow)
			{
				i64_actorid roleid = cur_guess_role();
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
}//namespace ngl
