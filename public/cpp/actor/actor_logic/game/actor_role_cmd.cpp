#include "actor_notice.h"
#include "actor_role.h"

namespace ngl
{
	bool actor_role::handle(message<pbnet::PROBUFF_NET_CMD>& adata)
	{
		pbnet::PROBUFF_NET_CMD& lparm = *adata.get_data();
		log_info()->print("cmd[{}]", lparm.m_cmd());

		std::vector<std::string> lvec;
		if (tools::splite(lparm.m_cmd().c_str(), "|", lvec) == false)
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
				}
			);
			handle_cmd::push("/lv", [](actor_role* role, const char* aparm)
				{
					role->m_info.change_lv(tools::lexical_cast<int>(aparm));
				}
			);
			handle_cmd::push("/gold", [](actor_role* role, const char* aparm)
				{
					role->m_info.change_gold(tools::lexical_cast<int>(aparm));
				}
			);
			handle_cmd::push("/silver", [](actor_role* role, const char* aparm)
				{
					role->m_info.change_silver(tools::lexical_cast<int>(aparm));
				}
			);
			handle_cmd::push("/chat", [](actor_role* role, const char* aparm)
				{
					// c 3 0 channelid			// 获取所有聊天记录
					// c 3 1 channelid "xxxx"	// 聊天发言	
					std::vector<std::string> lvec;
					if (tools::splite(aparm, "*", lvec) == false)
						return;
					if (lvec.size() >= 2)
					{
						auto pro = std::make_shared<pbnet::PROBUFF_NET_CHAT>();
						pro->set_m_type((pbnet::enum_logic_chat)tools::lexical_cast<int>(lvec[0].c_str()));
						pro->set_m_channelid(tools::lexical_cast<int>(lvec[1].c_str()));
						if (pro->m_type() == pbnet::chat_speak)
						{
							if (lvec.size() < 3)
							{
								return;
							}
							pro->set_m_content(lvec[2]);
						}
						message<pbnet::PROBUFF_NET_CHAT> lmessage(0, nullptr, pro);
						role->handle_forward<ACTOR_CHAT>(lmessage);
					}
				}
			);

			handle_cmd::push("/chatlist", [](actor_role* role, const char* aparm) 
				{
					auto pro = std::make_shared<pbnet::PROBUFF_NET_CHAT>();
					pro->set_m_type(pbnet::enum_logic_chat::get_chat_list);
					int lchannelid = 0;
					if (tools::splite(aparm, "*", lchannelid) == false)
						return;
					pro->set_m_channelid(lchannelid);
					message<pbnet::PROBUFF_NET_CHAT> lmessage(0, nullptr, pro);
					role->handle_forward<ACTOR_CHAT>(lmessage);
				}
			);

			handle_cmd::push("/switch", [](actor_role* role, const char* aparm)
				{
					auto pro = std::make_shared<pbnet::PROBUFF_NET_SWITCH_LINE>();
					tab_servers* tab = ttab_servers::find_first(GAME, [](tab_servers* atab)->bool
						{
							return atab->m_id != nconfig::m_nodeid;
						});

					if (tab == nullptr)
						return;

					pro->set_m_line(tab->m_tcount);
					message<pbnet::PROBUFF_NET_SWITCH_LINE> lmessage(0, nullptr, pro);
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
					role->send_client(role->id_guid(), pro);
				}
			);
			handle_cmd::push("/notices", [](actor_role* role, const char* aparm)
				{
					auto pro = std::make_shared<pbnet::PROBUFF_NET_NOTICE>();
					message<pbnet::PROBUFF_NET_NOTICE> lmessage(1, nullptr, pro);
					role->handle_forward<ACTOR_NOTICE>(lmessage);
				}
			);
			handle_cmd::push("/mails", [](actor_role* role, const char* aparm)
				{
					auto pro = std::make_shared<pbnet::PROBUFF_NET_MAIL_LIST>();
					message<pbnet::PROBUFF_NET_MAIL_LIST> lmessage(1, nullptr, pro);
					role->handle_forward<ACTOR_MAIL>(lmessage);
				}
			);

			handle_cmd::push("/create_family", [](actor_role* role, const char* aparm)
				{
					auto pro = std::make_shared<pbnet::PROBUFF_NET_CREATE_FAMIL>();
					pro->set_m_name(aparm);
					message<pbnet::PROBUFF_NET_CREATE_FAMIL> lmessage(1, nullptr, pro);
					role->handle_forward<ACTOR_FAMILY>(lmessage);
				}
			);

			handle_cmd::push("/family", [](actor_role* role, const char* aparm)
				{
					auto pro = std::make_shared<pbnet::PROBUFF_NET_FAMIL_LIST>();
					if(aparm != std::string(""))
						pro->set_m_familid(tools::lexical_cast<int64_t>(aparm));
					else
						pro->set_m_familid(-1);
					message<pbnet::PROBUFF_NET_FAMIL_LIST> lmessage(1, nullptr, pro);
					role->handle_forward<ACTOR_FAMILY>(lmessage);
				}
			);

			handle_cmd::push("/join_family", [](actor_role* role, const char* aparm)
				{
					auto pro = std::make_shared<pbnet::PROBUFF_NET_JOIN_FAMIL>();
					pro->set_m_familid(tools::lexical_cast<int64_t>(aparm));
					message<pbnet::PROBUFF_NET_JOIN_FAMIL> lmessage(1, nullptr, pro);
					role->handle_forward<ACTOR_FAMILY>(lmessage);
				}
			);

			handle_cmd::push("/leave_family", [](actor_role* role, const char* aparm)
				{
					auto pro = std::make_shared<pbnet::PROBUFF_NET_LEAVE_FAMIL>();
					pro->set_m_familid(tools::lexical_cast<int64_t>(aparm));
					message<pbnet::PROBUFF_NET_LEAVE_FAMIL> lmessage(1, nullptr, pro);
					role->handle_forward<ACTOR_FAMILY>(lmessage);
				}
			);

			handle_cmd::push("/change_familyname", [](actor_role* role, const char* aparm)
				{
					auto pro = std::make_shared<pbnet::PROBUFF_NET_CHANGE_FAMILNAME>();
					int64_t lfamilylid = 0;
					std::string lfamilylname;
					if (tools::splite(aparm, "*", lfamilylid, lfamilylname) == false)
						return;
					pro->set_m_familid(lfamilylid);
					pro->set_m_name(lfamilylname);
					message<pbnet::PROBUFF_NET_CHANGE_FAMILNAME> lmessage(1, nullptr, pro);
					role->handle_forward<ACTOR_FAMILY>(lmessage);
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