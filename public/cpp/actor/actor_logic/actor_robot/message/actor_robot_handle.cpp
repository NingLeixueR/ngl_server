#include "actor_robot.h"
#include "nregister.h"
#include "nforward.h"
namespace ngl
{
	bool actor_robot::handle(const message<np_thruput_test>& adata)
	{
		test_thruput::getInstance().change_rounds(adata.get_data()->m_rounds);
		return true;
	}
	bool actor_robot::handle(const message<pbexample::PROBUFF_EXAMPLE_GUESS_NUMBER_BOMB>& adata)
	{
		tools::print_json2proto(*adata.get_data(), true);
		return true;
	}
	bool actor_robot::handle(const message<pbexample::PROBUFF_EXAMPLE_GUESS_NUMBER_ERROR>& adata)
	{
		tools::print_json2proto(*adata.get_data(), true);
		return true;
	}
	bool actor_robot::handle(const message<pbexample::PROBUFF_EXAMPLE_GUESS_NUMBER_INFO>& adata)
	{
		tools::print_json2proto(*adata.get_data(), true);
		return true;
	}
	bool actor_robot::handle(const message<pbexample::PROBUFF_EXAMPLE_PLAY_CREATE>& adata)
	{
		tools::print_json2proto(*adata.get_data(), true);
		return true;
	}
	bool actor_robot::handle(const message<pbexample::PROBUFF_EXAMPLE_PLAY_ENTER_EXAMPLE_RESPONSE>& adata)
	{
		tools::print_json2proto(*adata.get_data(), true);
		return true;
	}
	bool actor_robot::handle(const message<pbexample::PROBUFF_EXAMPLE_PLAY_JOIN_RESPONSE>& adata)
	{
		tools::print_json2proto(*adata.get_data(), true);
		return true;
	}
	bool actor_robot::handle(const message<pbexample::PROBUFF_EXAMPLE_PLAY_MATCHING>& adata)
	{
		tools::print_json2proto(*adata.get_data(), true);
		return true;
	}
	bool actor_robot::handle(const message<pbexample::PROBUFF_EXAMPLE_PLAY_MATCHING_RESULT>& adata)
	{
		tools::print_json2proto(*adata.get_data(), true);
		return true;
	}
	bool actor_robot::handle(const message<pbnet::PROBUFF_NET_BAG_SYNC_RESPONSE>& adata)
	{
		return true;
	}
	bool actor_robot::handle(const message<pbnet::PROBUFF_NET_BAG_UPDATE>& adata)
	{
		return true;
	}
	bool actor_robot::handle(const message<pbnet::PROBUFF_NET_CHAT_RESPONSE>& adata)
	{
		auto lrecv = adata.get_data();
		if (lrecv->m_type() == pbnet::get_chat_list)
		{
			char lbuff[1024] = { 0 };
			for (const auto& item : lrecv->m_chatlist())
			{
				ngl::localtime::time2str(lbuff, 1024, item.m_utc(), "%y/%m/%d %H:%M:%S");
				log_error()->print("[{}:{}:{}] {}", nguid::area(item.m_roleid()), item.m_rolename(), lbuff, item.m_content());
			}
		}
		else if (lrecv->m_type() == pbnet::chat_speak)
		{
			//log_error()->print("{}", (lrecv->m_stat() ? "[发言成功]" : "[发言失败] "));
		}
		else if (lrecv->m_type() == pbnet::updata_speck)
		{
			char lbuff[1024] = { 0 };
			for (const auto& item : lrecv->m_chatlist())
			{
				ngl::localtime::time2str(lbuff, 1024, item.m_utc(), "%y/%m/%d %H:%M:%S");
				log_error()->print("[{}:{}:{}] {}", nguid::area(item.m_roleid()), item.m_rolename(), lbuff, item.m_content());
			}
		}
		return true;
	}
	bool actor_robot::handle(const message<pbnet::PROBUFF_NET_DELIVER_GOODS_RECHARGE>& adata)
	{
		tools::print_json2proto(*adata.get_data(), true);
		return true;
	}
	bool actor_robot::handle(const message<pbnet::PROBUFF_NET_ENTER_LEAVE_VIEW>& adata)
	{
		return true;
	}
	bool actor_robot::handle(const message<pbnet::PROBUFF_NET_ERROR>& adata)
	{
		tools::print_json2proto(*adata.get_data(), true);
		return true;
	}
	bool actor_robot::handle(const message<pbnet::PROBUFF_NET_FAMIL_CEDE_RESPONSE>& adata)
	{
		tools::print_json2proto(*adata.get_data(), true);
		return true;
	}
	bool actor_robot::handle(const message<pbnet::PROBUFF_NET_FAMIL_CHANGENAME_RESPONSE>& adata)
	{
		tools::print_json2proto(*adata.get_data(), true);
		return true;
	}
	bool actor_robot::handle(const message<pbnet::PROBUFF_NET_FAMIL_CREATE_RESPONSE>& adata)
	{
		tools::print_json2proto(*adata.get_data(), true);
		return true;
	}
	bool actor_robot::handle(const message<pbnet::PROBUFF_NET_FAMIL_JOIN_RESPONSE>& adata)
	{
		tools::print_json2proto(*adata.get_data(), true);
		return true;
	}
	bool actor_robot::handle(const message<pbnet::PROBUFF_NET_FAMIL_LEAVE_RESPONSE>& adata)
	{
		tools::print_json2proto(*adata.get_data(), true);
		return true;
	}
	bool actor_robot::handle(const message<pbnet::PROBUFF_NET_FAMIL_LIST_RESPONSE>& adata)
	{
		tools::print_json2proto(*adata.get_data(), true);
		return true;
	}
	bool actor_robot::handle(const message<pbnet::PROBUFF_NET_FAMIL_RATIFY_JOIN_RESPONSE>& adata)
	{
		tools::print_json2proto(*adata.get_data(), true);
		return true;
	}
	bool actor_robot::handle(const message<pbnet::PROBUFF_NET_FAMIL_SIGN_RESPONSE>& adata)
	{
		tools::print_json2proto(*adata.get_data(), true);
		return true;
	}
	bool actor_robot::handle(const message<pbnet::PROBUFF_NET_FRIEND_ADD_RESPONSE>& adata)
	{
		tools::print_json2proto(*adata.get_data(), true);
		return true;
	}
	bool actor_robot::handle(const message<pbnet::PROBUFF_NET_FRIEND_ERASE_RESPONSE>& adata)
	{
		tools::print_json2proto(*adata.get_data(), true);
		return true;
	}
	bool actor_robot::handle(const message<pbnet::PROBUFF_NET_FRIEND_RATIFY_ADD_RESPONSE>& adata)
	{
		tools::print_json2proto(*adata.get_data(), true);
		return true;
	}
	bool actor_robot::handle(const message<pbnet::PROBUFF_NET_FRIEND_RESPONSE>& adata)
	{
		tools::print_json2proto(*adata.get_data(), true);
		return true;
	}
	bool actor_robot::handle(const message<pbnet::PROBUFF_NET_GET_TIME_RESPONSE>& adata)
	{
		char lbuff[1024] = { 0 };
		ngl::localtime::time2str(lbuff, 1024, adata.get_data()->m_utc(), "%y/%m/%d %H:%M:%S");
		log_error()->print("[{}][{}]", m_data.m_brief().m_name(), lbuff);
		return true;
	}
	bool actor_robot::handle(const message<pbnet::PROBUFF_NET_KCPSESSION_RESPONSE>& adata)
	{
		const tab_servers* tab = ttab_servers::tab();
		m_kcpsessionmd5 = adata.get_data()->m_kcpsession();

		const tab_servers* tabgame = ttab_servers::tab("game", tab->m_area, 1);
		net_works const* lpworks = ttab_servers::nworks(ENET_KCP, tabgame);
		if (lpworks == nullptr)
			return false;
		return connect_kcp(m_kcp, lpworks->m_ip, lpworks->m_port);
	}
	bool actor_robot::handle(const message<pbnet::PROBUFF_NET_MAIL_DEL_RESPONSE>& adata)
	{
		tools::print_json2proto(*adata.get_data(), true);
		return true;
	}
	bool actor_robot::handle(const message<pbnet::PROBUFF_NET_MAIL_DRAW_RESPONSE>& adata)
	{
		tools::print_json2proto(*adata.get_data(), true);
		return true;
	}
	bool actor_robot::handle(const message<pbnet::PROBUFF_NET_MAIL_LIST_RESPONSE>& adata)
	{
		const pbnet::PROBUFF_NET_MAIL_LIST_RESPONSE& pro = *adata.get_data();
		char lbuff[1024] = { 0 };
		auto lstream = log_error();
		for (const auto& [_id, _mail] : pro.m_mail())
		{
			(*lstream) << "[####################################################################]" << std::endl;
			(*lstream) << "[mailid:	" << _mail.m_id() << "]" << std::endl;
			std::string lcontent;
			tools::to_asscii(_mail.m_content(), lcontent);
			(*lstream) << "[content:	" << lcontent << "]" << std::endl;
			ngl::localtime::time2str(lbuff, 1024, _mail.m_createutc(), "%y/%m/%d %H:%M:%S");
			(*lstream) << "[createutc:	" << lbuff << "]" << std::endl;
			(*lstream) << "[read:		" << (_mail.m_read() ? "yes" : "no") << "]" << std::endl;
			(*lstream) << "[draw:		" << (_mail.m_draw() ? "yes" : "no") << "]" << std::endl;
			(*lstream) << "[####################################################################]" << std::endl;
		}
		(*lstream).print("");
		return true;
	}
	bool actor_robot::handle(const message<pbnet::PROBUFF_NET_MAIL_READ_RESPONSE>& adata)
	{
		tools::print_json2proto(*adata.get_data(), true);
		return true;
	}
	bool actor_robot::handle(const message<pbnet::PROBUFF_NET_MSG_RESPONSE>& adata)
	{
		tools::print_json2proto(*adata.get_data(), true);
		return true;
	}
	bool actor_robot::handle(const message<pbnet::PROBUFF_NET_NOTICE_RESPONSE>& adata)
	{
		struct noticeitem
		{
			int64_t		m_id;			// 公告id 
			std::string m_notice;		// 内容
			std::string m_starttime;	// 开始时间
			std::string m_finishtime;	// 结束时间
			jsonfunc("id", m_id, "notice", m_notice, "starttime", m_starttime, "finishtime", m_finishtime)
		};
		struct notices
		{
			std::vector<noticeitem> m_notices;
			jsonfunc("notices", m_notices)
		};
		json_write ljson;
		notices lnotices;
		char lbuffstart[1024] = { 0 };
		char lbufffinish[1024] = { 0 };
		for (const auto& item : adata.get_data()->m_notices())
		{
			ngl::localtime::time2str(lbuffstart, 1024, item.m_starttime(), "%Y/%m/%d %H:%M:%S");
			ngl::localtime::time2str(lbufffinish, 1024, item.m_finishtime(), "%Y/%m/%d %H:%M:%S");
			lnotices.m_notices.push_back(
				noticeitem
				{
					.m_id = item.m_id(),
					.m_notice = item.m_notice(),
					.m_starttime = lbuffstart,
					.m_finishtime = lbufffinish,
				}
				);
		}
		ljson.write("notice_list", lnotices);
		std::string lstr;
		ljson.get(lstr);
		std::string lstrasscii;
		ngl::tools::to_asscii(lstr, lstrasscii);
		log_error()->print(
			"[{}:{}] {}",
			area(), m_data.m_brief().m_name(), lstrasscii
		);
		return true;
	}
	bool actor_robot::handle(const message<pbnet::PROBUFF_NET_RANKLIST_RESPONSE>& adata)
	{
		tools::print_json2proto(*adata.get_data(), true);
		return true;
	}
	bool actor_robot::handle(const message<pbnet::PROBUFF_NET_RECHARGE_RESPONSE>& adata)
	{
		return true;
	}
	bool actor_robot::handle(const message<pbnet::PROBUFF_NET_REWARD_ITEM_RESPONSE>& adata)
	{
		tools::print_json2proto(*adata.get_data(), true);
		return true;
	}
	bool actor_robot::handle(const message<pbnet::PROBUFF_NET_ROLESTAT>& adata)
	{
		tools::print_json2proto(*adata.get_data(), true);
		return true;
	}
	bool actor_robot::handle(const message<pbnet::PROBUFF_NET_ROLE_SYNC_RESPONSE>& adata)
	{
		Try
		{
			log_error()->print("[LOGIC_ROLE_SYNC:{}:{}]"
				, adata.get_data()->m_brief().m_name()
				, adata.get_data()->m_brief().m_lv()
			);
			m_data = *adata.get_data();
		}Catch
			return true;
	}
	bool actor_robot::handle(const message<pbnet::PROBUFF_NET_SWITCH_LINE_RESPONSE>& adata)
	{
		tools::print_json2proto(*adata.get_data(), true);
		return true;
	}
	bool actor_robot::handle(const message<pbnet::PROBUFF_NET_SYNC_ATTRIBUTE>& adata)
	{
		return true;
	}
	bool actor_robot::handle(const message<pbnet::PROBUFF_NET_SYNC_POSITION>& adata)
	{
		return true;
	}
	bool actor_robot::handle(const message<pbnet::PROBUFF_NET_SYNC_UNIT>& adata)
	{
		return true;
	}
	bool actor_robot::handle(const message<pbnet::PROBUFF_NET_TASK_RECEIVE_AWARD_RESPONSE>& adata)
	{
		return true;
	}
}//namespace ngl
