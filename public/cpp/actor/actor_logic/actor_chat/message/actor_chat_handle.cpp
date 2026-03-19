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
	bool actor_chat::handle(const message<mforward<pbnet::PROBUFF_NET_CHAT>>& adata)
	{
		const pbnet::PROBUFF_NET_CHAT& recv = *adata.get_data()->data();
		if (recv.mtype() == pbnet::ENUM_CHAT_SPEAK)
		{
			pbnet::PROBUFF_NET_CHAT_RESPONSE pro;
			pro.set_mtype(pbnet::ENUM_CHAT_SPEAK);
			pro.set_mchannelid(recv.mchannelid());
			pro.set_mstat(false);

			const tab_chat* ltab = ttab_chat::instance().tab(recv.mchannelid());
			if (ltab == nullptr)
			{
				send_client(adata.get_data()->identifier(), pro);
				return true;
			}

			int& llastspeakutc = m_lastspeakutc[adata.get_data()->identifier()];
			//int ltemputc = localtime::gettime() - llastspeakutc;
			//if (ltemputc < ltab->m_time)
			//{
			//	send_client(adata.m_data->identifier(), pro);
			//	return true;
			//}
			llastspeakutc = (int)localtime::gettime();

			const pbdb::db_brief* lpbrief = tdb_brief::nsp_cread<actor_chat>::instance(id_guid()).getconst(adata.get_data()->identifier());
			if (lpbrief == nullptr)
			{
				return true;
			}

			std::list<pbnet::chatitem>& lvec = m_update_chatitem[recv.mchannelid()];
			lvec.push_back(pbnet::chatitem());
			pbnet::chatitem& lchatitem = *lvec.rbegin();

			lchatitem.set_mrolename(lpbrief->mbase().mname());
			lchatitem.set_mutc((int)localtime::gettime());
			lchatitem.set_mcontent(recv.mcontent());
			lchatitem.set_mroleid(lpbrief->mid());

			std::list<pbnet::chatitem>& lschatitem = m_chatitem[recv.mchannelid()];
			lschatitem.push_back(lchatitem);
			if (lschatitem.size() > ltab->m_count)
			{
				lschatitem.pop_front();
			}

			pro.set_mstat(true);
			send_client(adata.get_data()->identifier(), pro);
		}
		else if (recv.mtype() == pbnet::ENUM_GET_CHAT_LIST)
		{
			pbnet::PROBUFF_NET_CHAT_RESPONSE pro;
			pro.set_mstat(false);
			pro.set_mtype(pbnet::ENUM_GET_CHAT_LIST);
			pro.set_mchannelid(recv.mchannelid());

			auto lpchanne = tools::findmap(m_chatitem, recv.mchannelid());
			if (lpchanne == nullptr)
			{
				send_client(adata.get_data()->identifier(), pro);
				return true;
			}
			for (pbnet::chatitem& item : *lpchanne)
			{
				*pro.add_mchatlist() = item;
			}
			pro.set_mstat(true);
			send_client(adata.get_data()->identifier(), pro);
			return true;
		}
		return true;
	}
}//namespace ngl