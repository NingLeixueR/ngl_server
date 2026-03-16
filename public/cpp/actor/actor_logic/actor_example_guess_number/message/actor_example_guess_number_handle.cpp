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

#include "actor/actor_logic/actor_example_guess_number/actor_example_guess_number.h"
#include "actor/actor_logic/actor_brief/actor_brief.h"
namespace ngl
{
	bool actor_example_guess_number::handle(const message<mforward<pbexample::PROBUFF_EXAMPLE_GUESS_NUMBER>>& adata)
	{
		const pbexample::PROBUFF_EXAMPLE_GUESS_NUMBER* lpdata = adata.get_data()->data();
		i64_actorid roleid = adata.get_data()->identifier();

		if (isfinish())
		{// End
			bomb();
			return true;
		}
		if (roleid != next_guess_role())
		{
			send_error(roleid, pbexample::E_GUESS_NUMBER_ERROR_NOT_GUESS);
			return true;
		}
		int32_t lnumber = lpdata->mguessnumber();
		if (lnumber >= m_maxnumber || lnumber <= m_minnumber)
		{
			send_error(roleid, pbexample::E_GUESS_NUMBER_ERROR_VALUE);
			return true;
		}

		if (lnumber == m_bombvalues)
		{// End
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
		{// End under personal
			set_finish(next_guess_role());
			bomb();
			return true;
		}

		next_setp();
		return true;
	}
	bool actor_example_guess_number::handle(const message<np_example_entergame_ready>& adata)
	{
		next_setp();
		return true;
	}
}//namespace ngl