#include "actor_role.h"
#include "splite.h"

namespace ngl
{
	bool actor_role::handle(message<pbnet::PROBUFF_NET_CMD>& adata)
	{
		pbnet::PROBUFF_NET_CMD& lparm = *adata.m_data;
		LogInfo("cmd[%]", lparm.m_cmd());

		std::vector<std::string> lvec;
		ngl::splite::division(lparm.m_cmd().c_str(), "|", lvec);

		static std::map<std::string, std::function<void(actor_role*,const char*)>> lcmd;
		if (lcmd.empty())
		{
			lcmd["/time"] = [this](actor_role* role, const char* aparm)
				{
					pbnet::PROBUFF_NET_GET_TIME pro;
					message<pbnet::PROBUFF_NET_GET_TIME> lmessage(0, nullptr, &pro);
					role->handle(lmessage);
				};
			lcmd["/name"] = [this](actor_role* role, const char* aparm)
				{
					role->m_info.change_name(aparm);
					role->sync_data_client();
				};
			lcmd["/lv"] = [this](actor_role* role, const char* aparm)
				{
					role->m_info.change_lv(boost::lexical_cast<int>(aparm));
					role->sync_data_client();
				};
			lcmd["/gold"] = [this](actor_role* role, const char* aparm)
				{
					role->m_info.change_gold(boost::lexical_cast<int>(aparm));
					role->sync_data_client();
				};
			lcmd["/silver"] = [this](actor_role* role, const char* aparm)
				{
					role->m_info.change_silver(boost::lexical_cast<int>(aparm));
					role->sync_data_client();
				};
			lcmd["/chat"] = [this](actor_role* role, const char* aparm)
				{
					// c 3 0 channelid			// 获取所有聊天记录
					// c 3 1 channelid "xxxx"	// 聊天发言	
					std::vector<std::string> lvec;
					ngl::splite::division(aparm, "*", lvec);
					if (lvec.size() >= 2)
					{
						pbnet::PROBUFF_NET_CHAT pro;
						pro.set_m_type((pbnet::enum_logic_chat)boost::lexical_cast<int>(lvec[0]));
						pro.set_m_channelid(boost::lexical_cast<int>(lvec[1]));
						if (pro.m_type() == pbnet::chat_speak)
						{
							if (lvec.size() < 3)
							{
								return;
							}
							pro.set_m_content(lvec[2]);
						}
						message<pbnet::PROBUFF_NET_CHAT> lmessage(0, nullptr, &pro);
						role->handle_forward<ACTOR_CHAT>(lmessage);
					}
				};
			lcmd["/switch"] = [this](actor_role* role, const char* aparm)
				{
					pbnet::PROBUFF_NET_SWITCH_LINE pro;
					tab_servers* tab = ttab_servers::find_first(GAME, [](tab_servers* atab)->bool
						{
							return atab->m_id != nconfig::m_nodeid;
						});

					if (tab == nullptr)
						return;

					pro.set_m_line(tab->m_tcount);
					message<pbnet::PROBUFF_NET_SWITCH_LINE> lmessage(0, nullptr, &pro);
					role->handle(lmessage);
				};
			lcmd["/createorder"] = [this](actor_role* role, const char* aparm)
				{
					std::string lorderid;
					role->createorder(lorderid, 1989);
					auto pro = std::make_shared<pbnet::PROBUFF_NET_ERROR_RESPONSE>();
					pro->set_m_errnum(0);
					pro->set_m_errmessage(lorderid);
					role->send2client(pro);
				};
		}
		
		std::function<void(actor_role*, const char*)>* lfun = tools::findmap(lcmd, lvec[0]);
		if (lfun != nullptr && *lfun != nullptr)
		{
			(*lfun)(this, lvec.size() >= 2? lvec[1].c_str() : "");
		}
		
		return true;
	}
}