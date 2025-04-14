#include "actor_notice.h"
#include "actor_role.h"

namespace ngl
{
	bool actor_role::handle(const message<pbnet::PROBUFF_NET_CMD>& adata)
	{
		const pbnet::PROBUFF_NET_CMD& lparm = *adata.get_data();
		log_info()->print("cmd [{}]", lparm.m_cmd());

		std::vector<std::string> lvec;
		if (tools::splite(lparm.m_cmd().c_str(), "|", lvec) == false)
		{
			return true;
		}
		if (handle_cmd::empty())
		{
			handle_cmd::push("/time", []([[maybe_unused]] const pack* apack, actor_role* role, [[maybe_unused]] const char* aparm)
				{
					pbnet::PROBUFF_NET_GET_TIME pro;
					message lmessage(0, apack, &pro);
					role->handle(lmessage);
				}
			);
			handle_cmd::push("/name", []([[maybe_unused]] const pack* apack, actor_role* role, [[maybe_unused]] const char* aparm)
				{
					role->m_info.rename(aparm);
				}
			);
			handle_cmd::push("/lv", []([[maybe_unused]] const pack* apack, actor_role* role, [[maybe_unused]] const char* aparm)
				{
					role->m_info.change_lv(tools::lexical_cast<int>(aparm));
				}
			);
			handle_cmd::push("/gold", []([[maybe_unused]] const pack* apack, actor_role* role, [[maybe_unused]] const char* aparm)
				{
					role->m_info.change_gold(tools::lexical_cast<int>(aparm));
				}
			);
			handle_cmd::push("/silver", []([[maybe_unused]] const pack* apack, actor_role* role, [[maybe_unused]] const char* aparm)
				{
					role->m_info.change_silver(tools::lexical_cast<int>(aparm));
				}
			);
			handle_cmd::push("/chat", []([[maybe_unused]] const pack* apack, actor_role* role, [[maybe_unused]] const char* aparm)
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
						message lmessage(0, nullptr, pro);
						role->handle_forward<ACTOR_CHAT>(lmessage);
					}
				}
			);
			handle_cmd::push("/switch", []([[maybe_unused]] const pack* apack, actor_role* role, [[maybe_unused]] const char* aparm)
				{
					auto pro = std::make_shared<pbnet::PROBUFF_NET_SWITCH_LINE>();
					const tab_servers* tab = ttab_servers::find_first(GAME, [](tab_servers* atab)->bool
						{
							return atab->m_id != nconfig::m_nodeid;
						});

					if (tab == nullptr)
					{
						return;
					}
					pro->set_m_line(tab->m_tcount);
					message lmessage(0, nullptr, pro);
					role->handle(lmessage);
				}
			);
			handle_cmd::push("/createorder", []([[maybe_unused]] const pack* apack, actor_role* role, [[maybe_unused]] const char* aparm)
				{
					int32_t lrechargeid = 0;
					if (tools::splite(aparm, "*", lrechargeid) == false)
					{
						return;
					}
					std::string lorderid;
					role->createorder(lorderid, lrechargeid);
					role->echo_msg(lorderid.c_str());
				}
			);
			handle_cmd::push("/notices", []([[maybe_unused]] const pack* apack, actor_role* role, [[maybe_unused]] const char* aparm)
				{
					auto pro = std::make_shared<pbnet::PROBUFF_NET_NOTICE>();
					message lmessage(1, nullptr, pro);
					role->handle_forward<ACTOR_NOTICE>(lmessage);
				}
			);
			handle_cmd::push("/mails", []([[maybe_unused]] const pack* apack, actor_role* role, [[maybe_unused]] const char* aparm)
				{
					auto pro = std::make_shared<pbnet::PROBUFF_NET_MAIL_LIST>();
					message lmessage(1, nullptr, pro);
					role->handle_forward<ACTOR_MAIL>(lmessage);
				}
			);
			handle_cmd::push("/create_family", []([[maybe_unused]] const pack* apack, actor_role* role, [[maybe_unused]] const char* aparm)
				{
					auto pro = std::make_shared<pbnet::PROBUFF_NET_CREATE_FAMIL>();
					pro->set_m_name(aparm);
					message lmessage(1, nullptr, pro);
					role->handle_forward<ACTOR_FAMILY>(lmessage);
				}
			);
			handle_cmd::push("/family", []([[maybe_unused]] const pack* apack, actor_role* role, [[maybe_unused]] const char* aparm)
				{
					auto pro = std::make_shared<pbnet::PROBUFF_NET_FAMIL_LIST>();
					if (aparm != std::string(""))
					{
						pro->set_m_familid(tools::lexical_cast<int64_t>(aparm));
					}
					else
					{
						pro->set_m_familid(-1);
					}
					message lmessage(1, nullptr, pro);
					role->handle_forward<ACTOR_FAMILY>(lmessage);
				}
			);
			handle_cmd::push("/join_family", []([[maybe_unused]] const pack* apack, actor_role* role, [[maybe_unused]] const char* aparm)
				{
					auto pro = std::make_shared<pbnet::PROBUFF_NET_JOIN_FAMIL>();
					pro->set_m_familid(tools::lexical_cast<int64_t>(aparm));
					pro->set_m_apply(true);
					message lmessage(1, nullptr, pro);
					role->handle_forward<ACTOR_FAMILY>(lmessage);
				}
			);
			handle_cmd::push("/cancel_join_family", []([[maybe_unused]] const pack* apack, actor_role* role, [[maybe_unused]] const char* aparm)
				{
					auto pro = std::make_shared<pbnet::PROBUFF_NET_JOIN_FAMIL>();
					pro->set_m_familid(tools::lexical_cast<int64_t>(aparm));
					pro->set_m_apply(false);
					message lmessage(1, nullptr, pro);
					role->handle_forward<ACTOR_FAMILY>(lmessage);
				}
			);
			handle_cmd::push("/ratify_family", []([[maybe_unused]] const pack* apack, actor_role* role, [[maybe_unused]] const char* aparm)
				{
					auto pro = std::make_shared<pbnet::PROBUFF_NET_RATIFY_JOIN_FAMIL>();
					int64_t lroleid = 0;
					int32_t lratify = 0;
					if (tools::splite(aparm, "*", lroleid, lratify) == false)
					{
						return;
					}
					pro->set_m_roleid(lroleid);
					pro->set_m_ratify(lratify);
					message lmessage(1, nullptr, pro);
					role->handle_forward<ACTOR_FAMILY>(lmessage);
				}
			);
			handle_cmd::push("/cede_family", []([[maybe_unused]] const pack* apack, actor_role* role, [[maybe_unused]] const char* aparm)
				{
					auto pro = std::make_shared<pbnet::PROBUFF_NET_CEDE_FAMIL>();
					int64_t lroleid = 0;
					if (tools::splite(aparm, "*", lroleid) == false)
					{
						return;
					}
					pro->set_m_roleid(lroleid);
					message lmessage(1, nullptr, pro);
					role->handle_forward<ACTOR_FAMILY>(lmessage);
				}
			);
			handle_cmd::push("/leave_family", []([[maybe_unused]] const pack* apack, actor_role* role, [[maybe_unused]] const char* aparm)
				{
					auto pro = std::make_shared<pbnet::PROBUFF_NET_LEAVE_FAMIL>();
					pro->set_m_familid(tools::lexical_cast<int64_t>(aparm));
					message lmessage(1, nullptr, pro);
					role->handle_forward<ACTOR_FAMILY>(lmessage);
				}
			);
			handle_cmd::push("/change_familyname", []([[maybe_unused]] const pack* apack, actor_role* role, [[maybe_unused]] const char* aparm)
				{
					auto pro = std::make_shared<pbnet::PROBUFF_NET_CHANGE_FAMILNAME>();
					int64_t lfamilylid = 0;
					std::string lfamilylname;
					if (tools::splite(aparm, "*", lfamilylid, lfamilylname) == false)
					{
						return;
					}
					pro->set_m_familid(lfamilylid);
					pro->set_m_name(lfamilylname);
					message lmessage(1, nullptr, pro);
					role->handle_forward<ACTOR_FAMILY>(lmessage);
				}
			);
			handle_cmd::push("/sign_family", []([[maybe_unused]] const pack* apack, actor_role* role, [[maybe_unused]] const char* aparm)
				{
					auto pro = std::make_shared<pbnet::PROBUFF_NET_FAMILSIGN>();
					int64_t lfamilylid = 0;
					if (tools::splite(aparm, "*", lfamilylid) == false)
					{
						return;
					}
					pro->set_m_familid(lfamilylid);
					message lmessage(1, nullptr, pro);
					role->handle_forward<ACTOR_FAMILY>(lmessage);
				}
			);
			handle_cmd::push("/ranklist", []([[maybe_unused]] const pack* apack, actor_role* role, [[maybe_unused]] const char* aparm)
				{
					auto pro = std::make_shared<pbnet::PROBUFF_NET_RANKLIST>();
					bool liscross = true;
					int32_t ltype = 0;
					int32_t lpage = 0;
					int32_t leverynum = 0;
					if (tools::splite(aparm, "*", liscross, ltype, lpage, leverynum) == false)
					{
						return;
					}
					pro->set_m_iscross(liscross);
					pro->set_m_type((pbdb::eranklist)ltype);
					pro->set_m_page(lpage);
					pro->set_m_everynum(leverynum);
					message lmessage(1, nullptr, pro);
					role->handle_forward<ACTOR_RANKLIST>(lmessage);
				}
			);
		}

		std::string& lcmd = lvec[0];
		std::ranges::transform(lcmd, lcmd.begin(), tolower);
		if (handle_cmd::function(lcmd, adata.m_pack, this, (lvec.size() >= 2 ? lvec[1].c_str() : "")) == false)
		{
			log_error()->print("actor_role cmd [{}] ERROR", lcmd);
		}
		return true;
	}
}//namespace ngl