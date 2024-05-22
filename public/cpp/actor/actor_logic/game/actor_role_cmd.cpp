#include "actor_notice.h"
#include "actor_role.h"
#include "splite.h"

namespace ngl
{
	bool actor_role::handle(message<pbnet::PROBUFF_NET_CMD>& adata)
	{
		pbnet::PROBUFF_NET_CMD& lparm = *adata.m_data;
		log_info()->print("cmd[{}]", lparm.m_cmd());

		std::vector<std::string> lvec;
		if (ngl::splite::func(lparm.m_cmd().c_str(), "|", lvec) == false)
			return true;
		if (handle_cmd::empty())
		{
			handle_cmd::push("/time", [](actor_role* role, const char* aparm)
				{
					pbnet::PROBUFF_NET_GET_TIME pro;
					message<pbnet::PROBUFF_NET_GET_TIME> lmessage(0, nullptr, &pro);
					role->handle(lmessage);
				}
			);
			handle_cmd::push("/name", [](actor_role* role, const char* aparm)
				{
					role->m_info.change_name(aparm);
					role->sync_data_client();
				}
			);
			handle_cmd::push("/lv", [](actor_role* role, const char* aparm)
				{
					role->m_info.change_lv(tools::lexical_cast<int>(aparm));
					role->sync_data_client();
				}
			);
			handle_cmd::push("/gold", [](actor_role* role, const char* aparm)
				{
					role->m_info.change_gold(tools::lexical_cast<int>(aparm));
					role->sync_data_client();
				}
			);
			handle_cmd::push("/silver", [](actor_role* role, const char* aparm)
				{
					role->m_info.change_silver(tools::lexical_cast<int>(aparm));
					role->sync_data_client();
				}
			);
			handle_cmd::push("/chat", [](actor_role* role, const char* aparm)
				{
					// c 3 0 channelid			// 获取所有聊天记录
					// c 3 1 channelid "xxxx"	// 聊天发言	
					std::vector<std::string> lvec;
					if (ngl::splite::func(aparm, "*", lvec) == false)
						return;
					if (lvec.size() >= 2)
					{
						pbnet::PROBUFF_NET_CHAT pro;
						pro.set_m_type((pbnet::enum_logic_chat)tools::lexical_cast<int>(lvec[0]));
						pro.set_m_channelid(tools::lexical_cast<int>(lvec[1]));
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
				}
			);

			handle_cmd::push("/chatlist", [](actor_role* role, const char* aparm) 
				{
					pbnet::PROBUFF_NET_CHAT pro;
					pro.set_m_type(pbnet::enum_logic_chat::get_chat_list);
					int lchannelid = 0;
					if (ngl::splite::func(aparm, "*", lchannelid) == false)
						return;
					pro.set_m_channelid(lchannelid);
					message<pbnet::PROBUFF_NET_CHAT> lmessage(0, nullptr, &pro);
					role->handle_forward<ACTOR_CHAT>(lmessage);
				}
			);

			handle_cmd::push("/switch", [](actor_role* role, const char* aparm)
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
				}
			);
			handle_cmd::push("/createorder", [](actor_role* role, const char* aparm)
				{
					std::string lorderid;
					role->createorder(lorderid, 1989);
					auto pro = std::make_shared<pbnet::PROBUFF_NET_ERROR_RESPONSE>();
					pro->set_m_errnum(0);
					pro->set_m_errmessage(lorderid);
					role->send2client(pro);
				}
			);
			handle_cmd::push("/notices", [](actor_role* role, const char* aparm)
				{
					pbnet::PROBUFF_NET_NOTICE lparm;
					message<pbnet::PROBUFF_NET_NOTICE> pro(1, nullptr, &lparm);
					role->handle_forward<ACTOR_NOTICE>(pro);
				}
			);
			handle_cmd::push("/mails", [](actor_role* role, const char* aparm)
				{
					pbnet::PROBUFF_NET_MAIL_LIST lparm;
					message<pbnet::PROBUFF_NET_MAIL_LIST> pro(1, nullptr, &lparm);
					role->handle_forward<ACTOR_MAIL>(pro);
				}
			);
		}

		std::string& lkey = lvec[0];
		std::transform(lkey.begin(), lkey.end(), lkey.begin(), tolower);
		if (handle_cmd::function(lkey, this, (lvec.size() >= 2 ? lvec[1].c_str() : "")) == false)
		{
			log_error()->print("actor_role cmd [{}] ERROR", lkey);
		}
		return true;
	}
}//namespace ngl