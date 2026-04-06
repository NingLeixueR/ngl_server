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
#include "actor/actor_logic/actor_brief/actor_brief.h"
#include "actor/actor_logic/actor_chat/actor_chat.h"
#include "actor/tab/ttab_chat.h"
namespace ngl
{
	bool actor_chat::handle([[maybe_unused]] const message<mforward<pbnet::PROBUFF_NET_CHAT>>& adata)
	{
		const auto* lparm = adata.get_data();
		const pbnet::PROBUFF_NET_CHAT& lrecv = *lparm->data();
		const i64_actorid lroleid = lparm->identifier();
		if (lrecv.mtype() == pbnet::ENUM_CHAT_SPEAK)
		{
			pbnet::PROBUFF_NET_CHAT_RESPONSE pro;
			pro.set_mtype(pbnet::ENUM_CHAT_SPEAK);
			pro.set_mchannelid(lrecv.mchannelid());
			pro.set_mstat(false);

			const tab_chat* ltab = ttab_chat::instance().tab(lrecv.mchannelid());
			if (ltab == nullptr)
			{
				send_client(lroleid, pro);
				return true;
			}

			auto llast_it = m_lastspeakutc.try_emplace(lroleid, 0).first;
			int& llastspeakutc = llast_it->second;
			//int ltemputc = tools::time::gettime() - llastspeakutc;
			//if (ltemputc < ltab->m_time)
			//{
			//	send_client(adata.m_data->identifier(), pro);
			//	return true;
			//}
			llastspeakutc = (int)tools::time::gettime();

			const pbdb::db_brief* lpbrief = tdb_brief::nsp_cread<actor_chat>::instance(id_guid()).getconst(lroleid);
			if (lpbrief == nullptr)
			{
				return true;
			}

			auto lvec_it = m_update_chatitem.try_emplace(lrecv.mchannelid()).first;
			std::list<pbnet::chatitem>& lvec = lvec_it->second;
			lvec.emplace_back();
			pbnet::chatitem& lchatitem = lvec.back();

			lchatitem.set_mrolename(lpbrief->mbase().mname());
			lchatitem.set_mutc((int)tools::time::gettime());
			lchatitem.set_mcontent(lrecv.mcontent());
			lchatitem.set_mroleid(lpbrief->mid());

			auto lchat_it = m_chatitem.try_emplace(lrecv.mchannelid()).first;
			std::list<pbnet::chatitem>& lschatitem = lchat_it->second;
			lschatitem.emplace_back(lchatitem);
			if (lschatitem.size() > ltab->m_count)
			{
				lschatitem.pop_front();
			}

			pro.set_mstat(true);
			send_client(lroleid, pro);
		}
		else if (lrecv.mtype() == pbnet::ENUM_GET_CHAT_LIST)
		{
			pbnet::PROBUFF_NET_CHAT_RESPONSE pro;
			pro.set_mstat(false);
			pro.set_mtype(pbnet::ENUM_GET_CHAT_LIST);
			pro.set_mchannelid(lrecv.mchannelid());

			const auto* lpchan = tools::findmap(m_chatitem, lrecv.mchannelid());
			if (lpchan == nullptr)
			{
				send_client(lroleid, pro);
				return true;
			}
			for (const pbnet::chatitem& litem : *lpchan)
			{
				*pro.add_mchatlist() = litem;
			}
			pro.set_mstat(true);
			send_client(lroleid, pro);
			return true;
		}
		return true;
	}
}//namespace ngl