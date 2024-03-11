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
						//.m_id = nconfig::m_nodeid,//设置此值说明每个进程都有一个聊天actor
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
		// 定时器
		actor::register_timer<actor_chat>(&actor_chat::timer_handle);
		// 协议注册
		register_actor<EPROTOCOL_TYPE_PROTOCOLBUFF, actor_chat>(
			false
			, dregister_fun_handle(actor_chat, mforward<pbnet::PROBUFF_NET_CHAT>)
			);

	}

	void actor_chat::init()
	{
		timerparm tparm;
		make_timerparm::make_interval(tparm, 2);
		set_timer(tparm);			
	}
	
	bool actor_chat::timer_handle(message<timerparm>& adata)
	{
		if (adata.m_data->m_type != timerparm::ET_INTERVAL_SEC)
			return true;
		
		//std::cout << "actor_chat::timer_handle ["; 
		//ngl::localtime::printf_time2str(0, "%y/%m/%d %H:%M:%S");
		//std::cout << "]" << std::endl;

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
			LogLocalError("Send All [pbnet::PROBUFF_NET_CHAT_RESPONSE]")
			send_allclient(pro);
			std::list<pbnet::chatitem>& ls = m_chatitem[_channelid];
			ls.insert(ls.end(), _update.begin(), _update.end());
			tab_chat* ltab = allcsv::tab<tab_chat>(_channelid);
			assert(ltab != nullptr);
			for (int i = ls.size() - ltab->m_count; i > 0; --i)
			{
				ls.pop_front();
			}
		}
		m_update_chatitem.clear();
		return true;
	}
}// namespace ngl