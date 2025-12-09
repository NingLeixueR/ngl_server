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
#include "actor_brief.h"
#include "actor_chat.h"
#include "ttab_chat.h"
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

			auto itor_channelid = m_chatitem.find(recv.mchannelid());
			if (itor_channelid == m_chatitem.end())
			{
				send_client(adata.get_data()->identifier(), pro);
				return true;
			}
			for (pbnet::chatitem& item : itor_channelid->second)
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
