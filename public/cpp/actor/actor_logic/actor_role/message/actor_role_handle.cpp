#include "ttab_specialid.h"
#include "actor_testlua.h"
#include "actor_events.h"
#include "manage_curl.h"
#include "nsp_server.h"
#include "nsp_write.h"
#include "nsp_read.h"
#include "actor_role.h"
#include "nregister.h"
#include "ttab_task.h"
#include "nforward.h"
#include "net.pb.h"
#include "drop.h"
#include "gcmd.h"
namespace ngl
{
	bool actor_role::handle(const message<mforward<np_gm>>& adata)
	{
		ngl::njson_read lojson(adata.get_data()->data()->m_json.c_str());
		std::string loperator;
		if (!njson::read(lojson, "operator", loperator))
		{
			return true;
		}

		if (handle_gm::empty())
		{
			handle_gm::add("pay") = [this](int id, ngl::njson_read& aos)
				{
					gcmd<int32_t> pro(id, "pay");
					struct pay
					{
						std::string m_orderid;
						int32_t m_rechargeid;
						dprotocol(pay, m_orderid, m_rechargeid)
					};
					pay lpay;
					if (!njson::read(aos, "data", lpay))
					{
						return;
					}
					pro.m_data = rechange(lpay.m_orderid, lpay.m_rechargeid, false, true);
				};

			handle_gm::add("gmrechange") = [this](int id, ngl::njson_read& aos)
				{
					gcmd<int32_t> pro(id, "gmrechange");
					int32_t lrechargeid;
					if (!njson::read(aos, "data", lrechargeid))
					{
						return;
					}
					std::string lorder;
					createorder(lorder, lrechargeid);
					pro.m_data = rechange(lorder, lrechargeid, true, true);
				};

			handle_gm::add("rechange") = [this](int id, ngl::njson_read& aos)
				{//actor_role::loginpay() callback
					gcmd<int32_t> pro(id, "rechange");
					prorechange lrechange;
					if (!njson::read(aos, "data", lrechange))
					{
						return;
					}

					pro.m_data = rechange(lrechange.m_orderid, lrechange.m_rechargeid, false, true);
				};

			// 禁言 lduration=0解封
			handle_gm::add("bantalk") = [this](int id, ngl::njson_read& aos)
				{
					gcmd<bool> pro(id, "bantalk", false);
					int32_t lduration;
					if (!njson::read(aos, "data", lduration))
					{
						return;
					}

					int lnow = (int)localtime::gettime();
					m_info.change_notalkutc(lnow + lduration);
					pro.m_data = true;
					log_error()->print("[{}] bantalk [{}]", id_guid(), tools::time2str(lnow + lduration));
				};
		}

		if (handle_gm::function(loperator, (int)adata.get_data()->identifier(), lojson) == false)
		{
			log_error()->print("GM actor_role php operator[{}] ERROR", loperator);
		}
		return true;
	}
	bool actor_role::handle(const message<mforward<np_operator_task>>& adata)
	{
		i64_actorid lactorid = adata.get_data()->identifier();
		const np_operator_task* recv = adata.get_data()->data();
		if (recv == nullptr)
		{
			return true;
		}
		if (recv->m_isreceive)
		{
			for (i64_actorid taskid : recv->m_taskids)
			{
				static_task::receive_task(this, (int32_t)taskid);
			}
		}
		else
		{
			for (i64_actorid taskid : recv->m_taskids)
			{
				static_task::finish_task(this, (int32_t)taskid);
				static_task::erase_task(this, (int32_t)taskid);
			}
		}
		log_error()->print("actor_role.np_operator_task {}:{}:{}:{}", 
			nguid(id_guid()), recv->m_msg, recv->m_isreceive?"receive":"remove", recv->m_taskids
		);
		auto pro = std::make_shared<mforward<np_operator_task_response>>();
		pro->data()->m_msg = recv->m_msg;
		actor::send_actor(lactorid, id_guid(), pro);
		return true;
	}
	bool actor_role::handle(const message<np_actor_disconnect_close>& adata)
	{
		erase_actor();
		return true;
	}
	bool actor_role::handle(const message<np_actor_senditem>& adata)
	{
		auto lparm = adata.get_data();
		local_remakes(this, lparm->m_src);
		m_bag.add_item(lparm->m_item);
		return true;
	}
	bool actor_role::handle(const message<np_eevents_logic_rolelogin>& adata)
	{
		return true;
	}
	bool actor_role::handle(const message<np_example_actorid>& adata)
	{
		m_example.first = adata.get_data()->m_type;
		m_example.second = adata.get_data()->m_actorexampleid;
		return true;
	}
	bool actor_role::handle(const message<pbnet::PROBUFF_NET_BAG_SYNC>& adata)
	{
		return true;
	}
	bool actor_role::handle(const message<pbnet::PROBUFF_NET_CHANGE_ANGLE>& adata)
	{
		return true;
	}
	bool actor_role::handle(const message<pbnet::PROBUFF_NET_CMD>& adata)
	{
		const pbnet::PROBUFF_NET_CMD& lparm = *adata.get_data();
		log_info()->print("cmd [{}]", lparm.mcmd());

		std::vector<std::string> lvec;
		if (tools::splite(lparm.mcmd().c_str(), "|", lvec) == false)
		{
			return true;
		}
		if (handle_cmd::empty())
		{
			handle_cmd::add("/time") = []([[maybe_unused]] const std::shared_ptr<pack>& apack, actor_role* role, [[maybe_unused]] const char* aparm)
				{
					pbnet::PROBUFF_NET_GET_TIME pro;
					message lmessage(0, apack, &pro);
					role->handle(lmessage);
				};

			handle_cmd::add("/name") = []([[maybe_unused]] const std::shared_ptr<pack>& apack, actor_role* role, [[maybe_unused]] const char* aparm)
				{
					role->m_info.rename(aparm);
					role->sync_data_client();
				};

			handle_cmd::add("/lv") = []([[maybe_unused]] const std::shared_ptr<pack>& apack, actor_role* role, [[maybe_unused]] const char* aparm)
				{
					role->m_info.change_lv(tools::lexical_cast<int>(aparm));
					role->sync_data_client();
				};

			handle_cmd::add("/gold") = []([[maybe_unused]] const std::shared_ptr<pack>& apack, actor_role* role, [[maybe_unused]] const char* aparm)
				{
					role->m_info.change_gold(tools::lexical_cast<int>(aparm));
					role->sync_data_client();
				};

			handle_cmd::add("/silver") = []([[maybe_unused]] const std::shared_ptr<pack>& apack, actor_role* role, [[maybe_unused]] const char* aparm)
				{
					role->m_info.change_silver(tools::lexical_cast<int>(aparm));
					role->sync_data_client();
				};

			handle_cmd::add("/chat") = []([[maybe_unused]] const std::shared_ptr<pack>& apack, actor_role* role, [[maybe_unused]] const char* aparm)
				{
					// c 3 0 channelid			// 获取所有聊天记录
					// c 3 1 channelid "xxxx"	// 聊天发言	
					std::vector<std::string> lvec;
					if (tools::splite(aparm, "*", lvec) == false)
						return;
					if (lvec.size() >= 2)
					{
						auto pro = std::make_shared<pbnet::PROBUFF_NET_CHAT>();
						pro->set_mtype((pbnet::ENUM_LOGIC_CHAT)tools::lexical_cast<int>(lvec[0].c_str()));
						pro->set_mchannelid(tools::lexical_cast<int>(lvec[1].c_str()));
						if (pro->mtype() == pbnet::ENUM_CHAT_SPEAK)
						{
							if (lvec.size() < 3)
							{
								return;
							}
							pro->set_mcontent(lvec[2]);
						}
						message lmessage(0, nullptr, pro);
						role->handle_forward<ACTOR_CHAT>(lmessage);
					}
				};

			handle_cmd::add("/switch") = []([[maybe_unused]] const std::shared_ptr<pack>& apack, actor_role* role, [[maybe_unused]] const char* aparm)
				{
					auto pro = std::make_shared<pbnet::PROBUFF_NET_SWITCH_LINE>();
					pro->set_mline(pro->mline()+1);
					message lmessage(0, nullptr, pro);
					role->handle(lmessage);
				};

			handle_cmd::add("/createorder") = []([[maybe_unused]] const std::shared_ptr<pack>& apack, actor_role* role, [[maybe_unused]] const char* aparm)
				{
					int32_t lrechargeid = 0;
					if (tools::splite(aparm, "*", lrechargeid) == false)
					{
						return;
					}
					std::string lorderid;
					role->createorder(lorderid, lrechargeid);
					role->echo_msg(lorderid.c_str());
				};

			handle_cmd::add("/notices") = []([[maybe_unused]] const std::shared_ptr<pack>& apack, actor_role* role, [[maybe_unused]] const char* aparm)
				{
					auto pro = std::make_shared<pbnet::PROBUFF_NET_NOTICE>();
					message lmessage(1, nullptr, pro);
					role->handle_forward<ACTOR_NOTICE>(lmessage);
				};

			handle_cmd::add("/mails") = []([[maybe_unused]] const std::shared_ptr<pack>& apack, actor_role* role, [[maybe_unused]] const char* aparm)
				{
					auto pro = std::make_shared<pbnet::PROBUFF_NET_MAIL_LIST>();
					message lmessage(1, nullptr, pro);
					role->handle_forward<ACTOR_MAIL>(lmessage);
				};

			handle_cmd::add("/create_family") = []([[maybe_unused]] const std::shared_ptr<pack>& apack, actor_role* role, [[maybe_unused]] const char* aparm)
				{
					auto pro = std::make_shared<pbnet::PROBUFF_NET_FAMIL_CREATE>();
					pro->set_mname(aparm);
					message lmessage(1, nullptr, pro);
					role->handle_forward<ACTOR_FAMILY>(lmessage);
				};

			handle_cmd::add("/family") = []([[maybe_unused]] const std::shared_ptr<pack>& apack, actor_role* role, [[maybe_unused]] const char* aparm)
				{
					auto pro = std::make_shared<pbnet::PROBUFF_NET_FAMIL_LIST>();
					if (aparm != std::string(""))
					{
						pro->set_mfamilid(tools::lexical_cast<int64_t>(aparm));
					}
					else
					{
						pro->set_mfamilid(-1);
					}
					message lmessage(1, nullptr, pro);
					role->handle_forward<ACTOR_FAMILY>(lmessage);
				};

			handle_cmd::add("/join_family") = []([[maybe_unused]] const std::shared_ptr<pack>& apack, actor_role* role, [[maybe_unused]] const char* aparm)
				{
					auto pro = std::make_shared<pbnet::PROBUFF_NET_FAMIL_JOIN>();
					pro->set_mfamilid(tools::lexical_cast<int64_t>(aparm));
					pro->set_mapply(true);
					message lmessage(1, nullptr, pro);
					role->handle_forward<ACTOR_FAMILY>(lmessage);
				};

			handle_cmd::add("/cancel_join_family") = []([[maybe_unused]] const std::shared_ptr<pack>& apack, actor_role* role, [[maybe_unused]] const char* aparm)
				{
					auto pro = std::make_shared<pbnet::PROBUFF_NET_FAMIL_JOIN>();
					pro->set_mfamilid(tools::lexical_cast<int64_t>(aparm));
					pro->set_mapply(false);
					message lmessage(1, nullptr, pro);
					role->handle_forward<ACTOR_FAMILY>(lmessage);
				};

			handle_cmd::add("/ratify_family") = []([[maybe_unused]] const std::shared_ptr<pack>& apack, actor_role* role, [[maybe_unused]] const char* aparm)
				{
					auto pro = std::make_shared<pbnet::PROBUFF_NET_FAMIL_RATIFY_JOIN>();
					int64_t lroleid = 0;
					int32_t lratify = 0;
					if (tools::splite(aparm, "*", lroleid, lratify) == false)
					{
						return;
					}
					pro->set_mroleid(lroleid);
					pro->set_mratify(lratify);
					message lmessage(1, nullptr, pro);
					role->handle_forward<ACTOR_FAMILY>(lmessage);
				};

			handle_cmd::add("/cede_family") = []([[maybe_unused]] const std::shared_ptr<pack>& apack, actor_role* role, [[maybe_unused]] const char* aparm)
				{
					auto pro = std::make_shared<pbnet::PROBUFF_NET_FAMIL_CEDE>();
					int64_t lroleid = 0;
					if (tools::splite(aparm, "*", lroleid) == false)
					{
						return;
					}
					pro->set_mroleid(lroleid);
					message lmessage(1, nullptr, pro);
					role->handle_forward<ACTOR_FAMILY>(lmessage);
				};

			handle_cmd::add("/leave_family") = []([[maybe_unused]] const std::shared_ptr<pack>& apack, actor_role* role, [[maybe_unused]] const char* aparm)
				{
					auto pro = std::make_shared<pbnet::PROBUFF_NET_FAMIL_LEAVE>();
					pro->set_mfamilid(tools::lexical_cast<int64_t>(aparm));
					message lmessage(1, nullptr, pro);
					role->handle_forward<ACTOR_FAMILY>(lmessage);
				};

			handle_cmd::add("/change_familyname") = []([[maybe_unused]] const std::shared_ptr<pack>& apack, actor_role* role, [[maybe_unused]] const char* aparm)
				{
					auto pro = std::make_shared<pbnet::PROBUFF_NET_FAMIL_CHANGENAME>();
					int64_t lfamilylid = 0;
					std::string lfamilylname;
					if (tools::splite(aparm, "*", lfamilylid, lfamilylname) == false)
					{
						return;
					}
					pro->set_mfamilid(lfamilylid);
					pro->set_mname(lfamilylname);
					message lmessage(1, nullptr, pro);
					role->handle_forward<ACTOR_FAMILY>(lmessage);
				};

			handle_cmd::add("/sign_family") = []([[maybe_unused]] const std::shared_ptr<pack>& apack, actor_role* role, [[maybe_unused]] const char* aparm)
				{
					auto pro = std::make_shared<pbnet::PROBUFF_NET_FAMIL_SIGN>();
					int64_t lfamilylid = 0;
					if (tools::splite(aparm, "*", lfamilylid) == false)
					{
						return;
					}
					pro->set_mfamilid(lfamilylid);
					message lmessage(1, nullptr, pro);
					role->handle_forward<ACTOR_FAMILY>(lmessage);
				};

			handle_cmd::add("/ranklist") = []([[maybe_unused]] const std::shared_ptr<pack>& apack, actor_role* role, [[maybe_unused]] const char* aparm)
				{
					auto pro = std::make_shared<pbnet::PROBUFF_NET_RANKLIST>();
					bool liscross = true;
					int32_t ltype = 0;
					int32_t lpage = 0;
					int32_t leverynum = 0;
					if (tools::splite(aparm, "*", liscross, ltype, lpage) == false)
					{
						return;
					}
					pro->set_miscross(liscross);
					pro->set_mtype((pbdb::eranklist)ltype);
					pro->set_mpage(lpage);
					message lmessage(1, nullptr, pro);
					role->handle_forward<ACTOR_RANKLIST>(lmessage);
				};

			handle_cmd::add("/exit") = []([[maybe_unused]] const std::shared_ptr<pack>& apack, actor_role* role, [[maybe_unused]] const char* aparm)
				{
					role->erase_actor();
				};

			handle_cmd::add("/testlua") = []([[maybe_unused]] const std::shared_ptr<pack>& apack, actor_role* role, [[maybe_unused]] const char* aparm)
				{
					int32_t lnumber = 0;
					if (tools::splite(aparm, "*", lnumber) == false)
					{
						return;
					}
					if (lnumber == 1)
					{
						auto pro = std::make_shared<pbnet::PROBUFF_NET_TESTLUA>();
						(*pro->mutable_mdata())[5] = "t5";
						(*pro->mutable_mdata())[4] = "t4";
						(*pro->mutable_mdata())[3] = "t3";
						*pro->mutable_mvalue() = "ribenrensb3";
						pro->set_mid(1989);
						message lmessage(1, nullptr, pro);
						role->handle_forward<ACTOR_TESTLUA>(lmessage);
					}
					else if (lnumber == 2)
					{
						auto pro = std::make_shared<np_testlua>();
						pro->m_name = "libo";
						pro->m_data[1] = "wac1";
						pro->m_data[2] = "lzs2";
						pro->m_data[3] = "lzm3";
						actor::send_actor(actor_testlua::actorid(), nguid::make(), pro);
					}
				};
		}

		std::string& lcmd = lvec[0];
		std::ranges::transform(lcmd, lcmd.begin(), tolower);
		if (handle_cmd::function(lcmd, adata.get_shared_pack(), this, (lvec.size() >= 2 ? lvec[1].c_str() : "")) == false)
		{
			log_error()->print("actor_role cmd [{}] ERROR", lcmd);
		}
		return true;
	}
	bool actor_role::handle(const message<pbnet::PROBUFF_NET_GET_TIME>& adata)
	{
		auto lpack = adata.get_pack();
		i64_actorid lrequest = lpack->m_head.get_request_actor();
		log_error()->print("{},NAME={}", guid(), m_info.name());
		if (lpack->m_protocol == ENET_KCP)
		{
			pbnet::PROBUFF_NET_GET_TIME_RESPONSE pro;
			pro.set_mutc((int32_t)localtime::gettime());
			sendkcp(pro, lrequest);
		}
		else
		{
			auto pro = std::make_shared<pbnet::PROBUFF_NET_GET_TIME_RESPONSE>();
			pro->set_mutc((int32_t)localtime::gettime());
			send_client(id_guid(), pro);
		}
		return true;
	}
	bool actor_role::handle(const message<pbnet::PROBUFF_NET_RECHARGE>& adata)
	{
		return true;
	}
	bool actor_role::handle(const message<pbnet::PROBUFF_NET_ROLE_SYNC>& adata)
	{
		sync_data_client();
		log_error()->print("[sync]###[{}]", m_info.name());
		return true;
	}
	bool actor_role::handle(const message<pbnet::PROBUFF_NET_SWITCH_LINE>& adata)
	{
		const tab_servers* tab = ttab_servers::instance().tab("game", nconfig::area());
		if (tab == nullptr)
		{
			return false;
		}
		
		int32_t lserverid = nnodeid::nodeid(tab->m_id, adata.get_data()->mline());
		i32_sessionid lsession = server_session::sessionid(lserverid);
		if (lsession == -1)
		{
			log_error()->print("PROBUFF_NET_SWITCH_LINE ERROR line[{}] severid[{}]", adata.get_data()->mline(), tab->m_id);
			return false;
		}
		np_actorswitch_process_role pro;
		pro.m_create = false;
		pro.m_gatewayid = m_gatewayid;
		actor_create::switch_process(id_guid(), nconfig::m_nodeid, lserverid, pro);
		return true;
	}
	bool actor_role::handle(const message<pbnet::PROBUFF_NET_TASK_RECEIVE_AWARD>& adata)
	{
		const tab_task* tab = ttab_task::instance().tab(adata.get_data()->mtaskid());
		if (tab == nullptr)
		{
			return true;
		}

		auto pro = std::make_shared<pbnet::PROBUFF_NET_TASK_RECEIVE_AWARD_RESPONSE>();
		pro->set_mtaskid(adata.get_data()->mtaskid());

		std::map<int, int> ldrop;
		std::string lsrc = std::format("task receive award");		
		if (!get_drop().use(tab->m_dropid, 1, id_guid(), lsrc, &ldrop))
		{
			log_error()->print("task:{} drop:{} fail!!!", adata.get_data()->mtaskid(), tab->m_dropid);
			return true;
		}
		tools::copy(ldrop, *pro->mutable_mdrop());
		m_bag.add_item(ldrop);
		return true;
	}
}//namespace ngl
