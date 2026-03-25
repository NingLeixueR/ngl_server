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
#include "actor/actor_logic/actor_friends/actor_friends.h"
#include "actor/actor_logic/actor_brief/actor_brief.h"
namespace ngl
{
	bool actor_friends::handle(const message<mforward<pbnet::PROBUFF_NET_FRIEND>>& adata)
	{
		const auto* lparm = adata.get_data();
		i64_actorid lroleid = lparm->identifier();
		m_friends.syncfriends(lroleid);
		return true;
	}
	bool actor_friends::handle(const message<mforward<pbnet::PROBUFF_NET_FRIEND_ADD>>& adata)
	{
		const auto* lparm = adata.get_data();
		const auto* lrecv = lparm->data();
		i64_actorid lroleid = lparm->identifier();
		i64_actorid lfriedid = lrecv->mfriedid();
		pbnet::PROBUFF_NET_FRIEND_ADD_RESPONSE pro;
		int32_t lstat = m_friends.addfriends(lroleid, lfriedid);
		pro.set_mfriedid(lfriedid);
		pro.set_mstat(lstat);
		send_client(lroleid, pro);
		return true;
	}
	bool actor_friends::handle(const message<mforward<pbnet::PROBUFF_NET_FRIEND_ERASE>>& adata)
	{
		const auto* lparm = adata.get_data();
		const auto* lrecv = lparm->data();
		i64_actorid lroleid = lparm->identifier();
		i64_actorid lfriedid = lrecv->mfriedid();

		pbnet::PROBUFF_NET_FRIEND_ERASE_RESPONSE pro;
		int32_t lstat = m_friends.erasefriends(lroleid, lfriedid);
		pro.set_mfriedid(lfriedid);
		pro.set_mstat(lstat);
		send_client(lroleid, pro);
		return true;
	}
	bool actor_friends::handle(const message<mforward<pbnet::PROBUFF_NET_FRIEND_RATIFY_ADD>>& adata)
	{
		const auto* lparm = adata.get_data();
		const auto* lrecv = lparm->data();
		i64_actorid lroleid = lparm->identifier();
		i64_actorid lfriedid = lrecv->mfriedid();
		bool lratify = lrecv->mratify();

		pbnet::PROBUFF_NET_FRIEND_RATIFY_ADD_RESPONSE pro;
		int32_t lstat = m_friends.ratifyfriends(lroleid, lfriedid, lratify);
		pro.set_mfriedid(lfriedid);
		pro.set_mratify(lratify);
		pro.set_mstat(lstat);
		send_client(lroleid, pro);
		return true;
	}
	bool actor_friends::handle(const message<np_eevents_logic_rolelogin>& adata)
	{
		const auto* lrecv = adata.get_data();
		std::vector<i64_actorid> lfriends;
		if (m_friends.get_friends(lrecv->m_actorid, lfriends))
		{
			pbnet::PROBUFF_NET_ROLESTAT pro;
			pro.set_mstat(pbnet::PROBUFF_NET_ROLESTAT::online);
			pro.set_mlogicstat(pbnet::PROBUFF_NET_ROLESTAT::friends);
			pro.set_mroleid(lrecv->m_actorid);
			send_client(lfriends, pro);
		}
		return true;
	}
	bool actor_friends::handle(const message<np_eevents_logic_roleoffline>& adata)
	{
		const auto* lrecv = adata.get_data();
		std::vector<i64_actorid> lfriends;
		if (m_friends.get_friends(lrecv->m_actorid, lfriends))
		{
			pbnet::PROBUFF_NET_ROLESTAT pro;
			pro.set_mstat(pbnet::PROBUFF_NET_ROLESTAT::offline);
			pro.set_mlogicstat(pbnet::PROBUFF_NET_ROLESTAT::friends);
			pro.set_mroleid(lrecv->m_actorid);
			send_client(lfriends, pro);
		}
		return true;
	}
}//namespace ngl
