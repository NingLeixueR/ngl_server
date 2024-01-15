#pragma once

#include "actor_manage.h"
#include "net.h"
#include "db_data.h"
#include "db.h"
#include "db_pool.h"
#include "db_manage.h"
#include "actor_db_client.h"
#include "actor_protocol.h"
#include "actor_timer.h"
#include "actor_create.h"
#include "db_modular.h"
#include "actor_roleitem.h"
#include "net.pb.h"
#include "manage_csv.h"

namespace ngl
{
	class actor_chat : public actor
	{
		actor_chat();

		struct roleitem
		{
			pbdb::db_brief m_info;
			int32_t m_lastspeakutc = 0;
		};
		using type_roleitems = roleitems<actor_chat, roleitem>;

		std::map<int, std::list<pbnet::chatitem>> m_chatitem;
		std::map<int, std::list<pbnet::chatitem>> m_update_chatitem;
	public:
		friend class actor_instance<actor_chat>;
		static actor_chat& getInstance()
		{
			return actor_instance<actor_chat>::instance();
		}

		static void actor_register();

		virtual ~actor_chat() {}

		virtual void loaddb_finish(bool adbishave) {}

		
		bool handle(
			message<mforward<pbnet::PROBUFF_NET_CHAT>>& adata)
		{
			
			pbnet::PROBUFF_NET_CHAT& recv = *adata.m_data->data();
			if (recv.m_type() == pbnet::chat_speak)
			{
				auto pro = std::make_shared<pbnet::PROBUFF_NET_CHAT_RESPONSE>();
				pro->set_m_type(pbnet::chat_speak);
				pro->set_m_channelid(recv.m_channelid());
				pro->set_m_stat(false);

				tab_chat* ltab = allcsv::tab<tab_chat>(recv.m_channelid());
				if (ltab == nullptr)
				{
					send_client(adata.m_data->identifier(), pro);
					return true;
				}

				roleitem* lroleitem = type_roleitems::get_roleinfo(adata.m_data->identifier());
				if (lroleitem == nullptr)
				{
					return true;
				}

				int ltemputc = localtime::gettime() - lroleitem->m_lastspeakutc;
				if (ltemputc < ltab->m_time)
				{
					send_client(adata.m_data->identifier(), pro);
					return true;
				}
				lroleitem->m_lastspeakutc = localtime::gettime();


				std::list<pbnet::chatitem>& lvec = m_update_chatitem[recv.m_channelid()];
				lvec.push_back(pbnet::chatitem());
				pbnet::chatitem& lchatitem = *lvec.rbegin();
				
				lchatitem.set_m_rolename(lroleitem->m_info.m_name());
				lchatitem.set_m_utc(localtime::gettime());
				lchatitem.set_m_content(recv.m_content());
				lchatitem.set_m_roleid(lroleitem->m_info.m_id());

				pro->set_m_stat(true);
				send_client(adata.m_data->identifier(), pro);
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
					send_client(adata.m_data->identifier(), pro);
					return true;
				}
				for (pbnet::chatitem& item : itor_channelid->second)
				{
					*pro->add_m_chatlist() = item;
				}

				auto itor_update_channelid = m_update_chatitem.find(recv.m_channelid());
				if (itor_update_channelid == m_update_chatitem.end())
				{
					send_client(adata.m_data->identifier(), pro);
					return true;
				}
				for (pbnet::chatitem& item : itor_update_channelid->second)
				{
					*pro->add_m_chatlist() = item;
				}

				pro->set_m_stat(true);
				send_client(adata.m_data->identifier(), pro);
				
				return true;
			}
			return true;
		}


		

		virtual void init();

		bool timer_handle(message<timerparm>& adata);
	private:
	};
}