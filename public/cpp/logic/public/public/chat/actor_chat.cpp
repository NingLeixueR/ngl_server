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
						.m_area = ttab_servers::tab()->m_area
					},
				.m_weight = 0x7fffffff,
			})
	{
		type_roleitems::init(
			[](std::map<i64_actorid, roleitem>& amap, pbdb::db_brief& ainfo)
			{
				roleitem& rinfo = amap[ainfo.m_id()];
				rinfo.m_info = ainfo;
			}
		);
	}

	void actor_chat::nregister()
	{
		// ��ʱ��
		actor::register_timer<actor_chat>(&actor_chat::timer_handle);
		// Э��ע��
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
	
	bool actor_chat::timer_handle(message<timerparm>& adata)
	{
		if (adata.get_data()->m_type != timerparm::ET_INTERVAL_SEC)
			return true;
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

	bool actor_chat::handle(message<mforward<pbnet::PROBUFF_NET_CHAT>>& adata)
	{
		pbnet::PROBUFF_NET_CHAT& recv = *adata.get_data()->data();
		//log_error()->print("recv [pbnet::PROBUFF_NET_CHAT] type:{}", recv.m_type());

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

			roleitem* lroleitem = type_roleitems::get_roleinfo(adata.get_data()->identifier());
			if (lroleitem == nullptr)
			{
				return true;
			}

			//int ltemputc = localtime::gettime() - lroleitem->m_lastspeakutc;
			//if (ltemputc < ltab->m_time)
			//{
			//	send_client(adata.m_data->identifier(), pro);
			//	return true;
			//}
			lroleitem->m_lastspeakutc = localtime::gettime();

			std::list<pbnet::chatitem>& lvec = m_update_chatitem[recv.m_channelid()];
			lvec.push_back(pbnet::chatitem());
			pbnet::chatitem& lchatitem = *lvec.rbegin();

			lchatitem.set_m_rolename(lroleitem->m_info.m_name());
			lchatitem.set_m_utc(localtime::gettime());
			lchatitem.set_m_content(recv.m_content());
			lchatitem.set_m_roleid(lroleitem->m_info.m_id());

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