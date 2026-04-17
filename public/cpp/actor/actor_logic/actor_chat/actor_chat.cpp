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
// File overview: Implements chat actor construction and channel dispatch.


#include "actor/actor_logic/actor_brief/actor_brief.h"
#include "actor/actor_logic/actor_chat/actor_chat.h"
#include "actor/generated/pb/net.pb.h"

namespace ngl
{
	actor_chat::actor_chat() :
		actor(
			actorparm
			{
				.m_parm
					{
						.m_type = ACTOR_CHAT,
						.m_area = tab_self_area
					},
				.m_weight = 0x7fffffff,
			})
	{
	}

	ENUM_ACTOR actor_chat::actor_type()
	{
		return ACTOR_CHAT;
	}

	i64_actorid actor_chat::actorid()
	{
		return nguid::make(ACTOR_CHAT, tab_self_area, nguid::none_actordataid());
	}

	void actor_chat::init()
	{
		// Bind the DB structure: DB.set(this);

		// Set up the timer_handle timer.
		np_timerparm tparm;
		if (make_timerparm::make_interval(tparm, 2) == false)
		{
			log_error()->print("actor_chat::init() make_timerparm::make_interval(tparm, 2) == false!!!");
			return;
		}
		set_timer(tparm);

		std::set<i32_fieldnumber> lfieldset;
		tdb_brief::nsp_cread<actor_chat>::instance_readall(this, tools::pb_field::field_number<pbdb::db_brief>(lfieldset, "mid", "mbase"));
	}

	void actor_chat::erase_actor_before()
	{
		tdb_brief::nsp_cread<actor_chat>::instance(id_guid()).exit();
	}

	void actor_chat::loaddb_finish(pbdb::ENUM_DB atype, enum_dbstat astat)
	{
		if (atype != pbdb::ENUM_DB::ENUM_DB_ALL)
		{
			log_error()->print("actor_chat::loaddb_finish [{}:{}]", (int32_t)atype, (int32_t)astat);
			return;
		}
	}

	void actor_chat::nregister()
	{
		// Timer.
		actor::register_timer<actor_chat>(&actor_chat::timer_handle);

		// Bind custom np_ messages.
		register_handle<actor_chat
			, mforward<pbnet::PROBUFF_NET_CHAT>
		>();
	}
	
	bool actor_chat::timer_handle([[maybe_unused]] const message<np_timerparm>& adata)
	{
		if (adata.get_data()->m_type != E_ACTOR_TIMER::ET_INTERVAL_SEC)
		{
			return true;
		}
		pbnet::PROBUFF_NET_CHAT_RESPONSE pro;
		pro.set_mstat(true);
		pro.set_mtype(pbnet::ENUM_UPDATA_SPEAK);

		for (const auto& lpair : m_update_chatitem)
		{
			pro.set_mchannelid(lpair.first);
			pro.clear_mchatlist();
			for (const pbnet::chatitem& litem : lpair.second)
			{
				*pro.add_mchatlist() = litem;
			}
			send_client(pro, ENET_KCP);
		}
		m_update_chatitem.clear();
		return true;
	}
}// namespace ngl
