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
// File overview: Implements mail actor construction and mailbox management.


#include "actor/actor_logic/actor_mail/actor_mail.h"

namespace ngl
{
	actor_mail::actor_mail() :
		actor(
			actorparm
			{
				.m_parm
				{
					.m_type = ACTOR_MAIL,
					.m_area = tab_self_area,
					.m_manage_dbclient = true
				},
				.m_weight = 0x7fffffff,
				.m_broadcast = true,
			})
	{
	}

	ENUM_ACTOR actor_mail::actor_type()
	{
		return ACTOR_MAIL;
	}

	i64_actorid actor_mail::actorid(i16_area area)
	{
		return nguid::make(actor_type(), area, nguid::none_actordataid());
	}

	void actor_mail::init()
	{
		// Bind the DB structure: DB.set(this);
		m_mails.set(this);

		// Set up the timer_handle timer.
		/*np_timerparm tparm;
		if (make_timerparm::make_interval(tparm, 2) == false)
		{
			log_error()->print("actor_mail::init() make_timerparm::make_interval(tparm, 2) == false!!!");
			return;
		}
		set_timer(tparm);
		*/

		m_drop.init(this, {});
	}

	void actor_mail::erase_actor_before()
	{
		m_drop.exit();
	}
	
	void actor_mail::loaddb_finish(pbdb::ENUM_DB atype, enum_dbstat astat)
	{
		if (atype != pbdb::ENUM_DB::ENUM_DB_ALL)
		{
			log_error()->print("actor_mail::loaddb_finish [{}:{}]", (int32_t)atype, (int32_t)astat);
			return;
		}
	}

	void actor_mail::nregister()
	{
		// Timer.
		actor::register_timer<actor_mail>(&actor_mail::timer_handle);

		// Bind custom np_ messages.
		register_handle<actor_mail
			, np_actor_addmail
			, mforward<np_gm>
			, mforward<pbnet::PROBUFF_NET_MAIL_LIST>
			, mforward<pbnet::PROBUFF_NET_MAIL_READ>
			, mforward<pbnet::PROBUFF_NET_MAIL_DRAW>
			, mforward<pbnet::PROBUFF_NET_MAIL_DEL>
		>();
	}

	bool actor_mail::sendmail(i64_actorid aactorid, int32_t amailid, int32_t adropid, const std::string& aparm)
	{
		auto pro = std::make_shared<np_actor_addmail>();
		pro->m_roleid = aactorid;
		pro->m_tid = amailid;
		pro->m_dropid = adropid;
		pro->m_parm = aparm;
		actor::send_actor(actorid(nguid::area(aactorid)), nguid::make(), pro);
		return true;
	}

	bool actor_mail::timer_handle([[maybe_unused]] const message<np_timerparm>& adata)
	{
		return true;
	}
}// namespace ngl
