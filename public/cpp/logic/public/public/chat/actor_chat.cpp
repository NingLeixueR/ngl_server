#include "actor_brief.h"
#include "actor_chat.h"
#include "net.pb.h"

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
		std::set<i64_actorid> ldataid;
		tdb_brief::nsp_cli<actor_chat>::init(actor_brief::actor_type(), this, ldataid);
	}

	void actor_chat::nregister()
	{
		// 定时器
		actor::register_timer<actor_chat>(&actor_chat::timer_handle);
		// 协议注册
		register_handle_proto<actor_chat>::func<
			mforward<pbnet::PROBUFF_NET_CHAT>
		>(false);
	}

	void actor_chat::init()
	{
		timerparm tparm;
		if (make_timerparm::make_interval(tparm, 2) == false)
		{
			log_error()->print("actor_chat::init() make_timerparm::make_interval(tparm, 2) == false!!!");
			return;
		}
		set_timer(tparm);			
	}
	
	bool actor_chat::timer_handle(const message<timerparm>& adata)
	{
		if (adata.get_data()->m_type != timerparm::ET_INTERVAL_SEC)
		{
			return true;
		}
		auto pro = std::make_shared<pbnet::PROBUFF_NET_CHAT_RESPONSE>();
		pro->set_m_stat(true);
		pro->set_m_type(pbnet::updata_speck);

		for (const auto& [_channelid, _update] : m_update_chatitem)
		{
			pro->set_m_channelid(_channelid);
			pro->clear_m_chatlist();
			for (const pbnet::chatitem& item : _update)
			{
				*pro->add_m_chatlist() = item;
			}
			send_allclient(pro);
		}
		m_update_chatitem.clear();
		return true;
	}

	bool actor_chat::handle(const message<mforward<pbnet::PROBUFF_NET_CHAT>>& adata)
	{
		const pbnet::PROBUFF_NET_CHAT& recv = *adata.get_data()->data();
		if (recv.m_type() == pbnet::chat_speak)
		{
			auto pro = std::make_shared<pbnet::PROBUFF_NET_CHAT_RESPONSE>();
			pro->set_m_type(pbnet::chat_speak);
			pro->set_m_channelid(recv.m_channelid());
			pro->set_m_stat(false);

			tab_chat* ltab = allcsv::tab<tab_chat>(recv.m_channelid());
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
			llastspeakutc = localtime::gettime();
			
			const pbdb::db_brief* lpbrief = tdb_brief::nsp_cli<actor_chat>::get(adata.get_data()->identifier());
			if (lpbrief == nullptr)
			{
				return true;
			}

			std::list<pbnet::chatitem>& lvec = m_update_chatitem[recv.m_channelid()];
			lvec.push_back(pbnet::chatitem());
			pbnet::chatitem& lchatitem = *lvec.rbegin();

			lchatitem.set_m_rolename(lpbrief->m_name());
			lchatitem.set_m_utc(localtime::gettime());
			lchatitem.set_m_content(recv.m_content());
			lchatitem.set_m_roleid(lpbrief->m_id());

			std::list<pbnet::chatitem>& lschatitem = m_chatitem[recv.m_channelid()];
			lschatitem.push_back(lchatitem);
			if (lschatitem.size() > ltab->m_count)
			{
				lschatitem.pop_front();
			}

			pro->set_m_stat(true);
			send_client(adata.get_data()->identifier(), pro);
		}
		else if (recv.m_type() == pbnet::get_chat_list)
		{
			auto pro = std::make_shared<pbnet::PROBUFF_NET_CHAT_RESPONSE>();
			pro->set_m_stat(false);
			pro->set_m_type(pbnet::get_chat_list);
			pro->set_m_channelid(recv.m_channelid());

			auto itor_channelid = m_chatitem.find(recv.m_channelid());
			if (itor_channelid == m_chatitem.end())
			{
				send_client(adata.get_data()->identifier(), pro);
				return true;
			}
			for (pbnet::chatitem& item : itor_channelid->second)
			{
				*pro->add_m_chatlist() = item;
			}
			pro->set_m_stat(true);
			send_client(adata.get_data()->identifier(), pro);
			return true;
		}
		return true;
	}
}// namespace ngl