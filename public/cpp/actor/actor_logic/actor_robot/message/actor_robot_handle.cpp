#include "actor_robot.h"
#include "nregister.h"
#include "nforward.h"
namespace ngl
{
	bool actor_robot::handle(const message<np_thruput_test>& adata)
	{
		test_thruput::instance().change_rounds(adata.get_data()->m_rounds);
		return true;
	}
	bool actor_robot::handle(const message<pbexample::PROBUFF_EXAMPLE_GUESS_NUMBER_BOMB>& adata)
	{
		handle_print<true>(adata);
		return true;
	}
	bool actor_robot::handle(const message<pbexample::PROBUFF_EXAMPLE_GUESS_NUMBER_ERROR>& adata)
	{
		handle_print<true>(adata);
		return true;
	}
	bool actor_robot::handle(const message<pbexample::PROBUFF_EXAMPLE_GUESS_NUMBER_INFO>& adata)
	{
		handle_print<true>(adata);
		return true;
	}
	bool actor_robot::handle(const message<pbexample::PROBUFF_EXAMPLE_PLAY_CREATE>& adata)
	{
		handle_print<true>(adata);
		return true;
	}
	bool actor_robot::handle(const message<pbexample::PROBUFF_EXAMPLE_PLAY_ENTER_EXAMPLE_RESPONSE>& adata)
	{
		handle_print<true>(adata);
		return true;
	}
	bool actor_robot::handle(const message<pbexample::PROBUFF_EXAMPLE_PLAY_JOIN_RESPONSE>& adata)
	{
		handle_print<true>(adata);
		return true;
	}
	bool actor_robot::handle(const message<pbexample::PROBUFF_EXAMPLE_PLAY_MATCHING>& adata)
	{
		handle_print<true>(adata);
		return true;
	}
	bool actor_robot::handle(const message<pbexample::PROBUFF_EXAMPLE_PLAY_MATCHING_RESULT>& adata)
	{
		handle_print<true>(adata);
		return true;
	}
	bool actor_robot::handle(const message<pbnet::PROBUFF_NET_BAG_SYNC_RESPONSE>& adata)
	{
		handle_print<true>(adata);
		return true;
	}
	bool actor_robot::handle(const message<pbnet::PROBUFF_NET_BAG_UPDATE>& adata)
	{
		handle_print<true>(adata);
		return true;
	}
	bool actor_robot::handle(const message<pbnet::PROBUFF_NET_CHAT_RESPONSE>& adata)
	{
		auto lrecv = adata.get_data();
		if (lrecv->mtype() == pbnet::ENUM_GET_CHAT_LIST)
		{
			char lbuff[1024] = { 0 };
			for (const auto& item : lrecv->mchatlist())
			{
				ngl::localtime::time2str(lbuff, 1024, item.mutc(), "%y/%m/%d %H:%M:%S");
				log_error()->print("[{}:{}:{}] {}", nguid::area(item.mroleid()), item.mrolename(), lbuff, item.mcontent());
			}
		}
		else if (lrecv->mtype() == pbnet::ENUM_CHAT_SPEAK)
		{
			//log_error()->print("{}", (lrecv->m_stat() ? "[发言成功]" : "[发言失败] "));
		}
		else if (lrecv->mtype() == pbnet::ENUM_UPDATA_SPEAK)
		{
			char lbuff[1024] = { 0 };
			for (const auto& item : lrecv->mchatlist())
			{
				ngl::localtime::time2str(lbuff, 1024, item.mutc(), "%y/%m/%d %H:%M:%S");
				log_error()->print("[{}:{}:{}] {}", nguid::area(item.mroleid()), item.mrolename(), lbuff, item.mcontent());
			}
		}
		return true;
	}
	bool actor_robot::handle(const message<pbnet::PROBUFF_NET_DELIVER_GOODS_RECHARGE>& adata)
	{
		handle_print<true>(adata);
		return true;
	}
	bool actor_robot::handle(const message<pbnet::PROBUFF_NET_ENTER_LEAVE_VIEW>& adata)
	{
		handle_print<true>(adata);
		return true;
	}
	bool actor_robot::handle(const message<pbnet::PROBUFF_NET_ERROR>& adata)
	{
		handle_print<true>(adata);
		return true;
	}
	bool actor_robot::handle(const message<pbnet::PROBUFF_NET_FAMIL_CEDE_RESPONSE>& adata)
	{
		handle_print<true>(adata);
		return true;
	}
	bool actor_robot::handle(const message<pbnet::PROBUFF_NET_FAMIL_CHANGENAME_RESPONSE>& adata)
	{
		handle_print<true>(adata);
		return true;
	}
	bool actor_robot::handle(const message<pbnet::PROBUFF_NET_FAMIL_CREATE_RESPONSE>& adata)
	{
		handle_print<true>(adata);
		return true;
	}
	bool actor_robot::handle(const message<pbnet::PROBUFF_NET_FAMIL_INFO_RESPONSE>& adata)
	{
		handle_print<true>(adata);
		return true;
	}
	bool actor_robot::handle(const message<pbnet::PROBUFF_NET_FAMIL_JOIN_RESPONSE>& adata)
	{
		handle_print<true>(adata);
		return true;
	}
	bool actor_robot::handle(const message<pbnet::PROBUFF_NET_FAMIL_LEAVE_RESPONSE>& adata)
	{
		handle_print<true>(adata);
		return true;
	}
	bool actor_robot::handle(const message<pbnet::PROBUFF_NET_FAMIL_LIST_RESPONSE>& adata)
	{
		handle_print<true>(adata);
		return true;
	}
	bool actor_robot::handle(const message<pbnet::PROBUFF_NET_FAMIL_RATIFY_JOIN_RESPONSE>& adata)
	{
		handle_print<true>(adata);
		return true;
	}
	bool actor_robot::handle(const message<pbnet::PROBUFF_NET_FAMIL_SIGN_RESPONSE>& adata)
	{
		handle_print<true>(adata);
		return true;
	}
	bool actor_robot::handle(const message<pbnet::PROBUFF_NET_FRIEND_ADD_RESPONSE>& adata)
	{
		handle_print<true>(adata);
		return true;
	}
	bool actor_robot::handle(const message<pbnet::PROBUFF_NET_FRIEND_ERASE_RESPONSE>& adata)
	{
		handle_print<true>(adata);
		return true;
	}
	bool actor_robot::handle(const message<pbnet::PROBUFF_NET_FRIEND_RATIFY_ADD_RESPONSE>& adata)
	{
		handle_print<true>(adata);
		return true;
	}
	bool actor_robot::handle(const message<pbnet::PROBUFF_NET_FRIEND_RESPONSE>& adata)
	{
		handle_print<true>(adata);
		return true;
	}
	bool actor_robot::handle(const message<pbnet::PROBUFF_NET_GET_TIME_RESPONSE>& adata)
	{
		char lbuff[1024] = { 0 };
		ngl::localtime::time2str(lbuff, 1024, adata.get_data()->mutc(), "%y/%m/%d %H:%M:%S");
		log_error()->print("[{}][{}]", m_data.mbrief().mname(), lbuff);
		return true;
	}
	bool actor_robot::handle(const message<pbnet::PROBUFF_NET_KCPSESSION_RESPONSE>& adata)
	{
		const tab_servers* tab = ttab_servers::instance().tab();
		m_kcpsessionmd5 = adata.get_data()->mkcpsession();

		const tab_servers* tabgame = ttab_servers::instance().tab("game", tab->m_area, 1);
		net_works const* lpworks = ttab_servers::instance().nworks(ENET_KCP, tabgame);
		if (lpworks == nullptr)
		{
			return false;
		}
		return connect_kcp(m_kcp, lpworks->m_ip, lpworks->m_port);
	}
	bool actor_robot::handle(const message<pbnet::PROBUFF_NET_MAIL_DEL_RESPONSE>& adata)
	{
		handle_print<true>(adata);
		return true;
	}
	bool actor_robot::handle(const message<pbnet::PROBUFF_NET_MAIL_DRAW_RESPONSE>& adata)
	{
		handle_print<true>(adata);
		return true;
	}
	bool actor_robot::handle(const message<pbnet::PROBUFF_NET_MAIL_LIST_RESPONSE>& adata)
	{
		handle_print<true>(adata);
		return true;
	}
	bool actor_robot::handle(const message<pbnet::PROBUFF_NET_MAIL_READ_RESPONSE>& adata)
	{
		handle_print<true>(adata);
		return true;
	}
	bool actor_robot::handle(const message<pbnet::PROBUFF_NET_MSG_RESPONSE>& adata)
	{
		handle_print<true>(adata);
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
			def_jsonfunction("id", m_id, "notice", m_notice, "starttime", m_starttime, "finishtime", m_finishtime)
		};
		struct notices
		{
			std::vector<noticeitem> m_notices;
			def_jsonfunction("notices", m_notices)
		};
		json_write ljson;
		notices lnotices;
		char lbuffstart[1024] = { 0 };
		char lbufffinish[1024] = { 0 };
		for (const auto& item : adata.get_data()->mnotices())
		{
			ngl::localtime::time2str(lbuffstart, 1024, item.mstarttime(), "%Y/%m/%d %H:%M:%S");
			ngl::localtime::time2str(lbufffinish, 1024, item.mfinishtime(), "%Y/%m/%d %H:%M:%S");
			lnotices.m_notices.push_back(
				noticeitem
				{
					.m_id = item.mid(),
					.m_notice = item.mnotice(),
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
			area(), m_data.mbrief().mname(), lstrasscii
		);
		return true;
	}
	bool actor_robot::handle(const message<pbnet::PROBUFF_NET_RANKLIST_RESPONSE>& adata)
	{
		handle_print<true>(adata);
		return true;
	}
	bool actor_robot::handle(const message<pbnet::PROBUFF_NET_RECHARGE_RESPONSE>& adata)
	{
		handle_print<true>(adata);
		return true;
	}
	bool actor_robot::handle(const message<pbnet::PROBUFF_NET_REWARD_ITEM_RESPONSE>& adata)
	{
		handle_print<true>(adata);
		return true;
	}
	bool actor_robot::handle(const message<pbnet::PROBUFF_NET_ROLESTAT>& adata)
	{
		handle_print<true>(adata);
		return true;
	}
	bool actor_robot::handle(const message<pbnet::PROBUFF_NET_ROLE_SYNC_RESPONSE>& adata)
	{
		log_error()->print("[LOGIC_ROLE_SYNC:{}:{}]"
			, adata.get_data()->mbrief().mname()
			, adata.get_data()->mbrief().mlv()
		);
		m_data = *adata.get_data();
		return true;
	}
	bool actor_robot::handle(const message<pbnet::PROBUFF_NET_SWITCH_LINE_RESPONSE>& adata)
	{
		handle_print<true>(adata);
		return true;
	}
	bool actor_robot::handle(const message<pbnet::PROBUFF_NET_SYNC_ATTRIBUTE>& adata)
	{
		handle_print<true>(adata);
		return true;
	}
	bool actor_robot::handle(const message<pbnet::PROBUFF_NET_SYNC_POSITION>& adata)
	{
		handle_print<true>(adata);
		return true;
	}
	bool actor_robot::handle(const message<pbnet::PROBUFF_NET_SYNC_UNIT>& adata)
	{
		handle_print<true>(adata);
		return true;
	}
	bool actor_robot::handle(const message<pbnet::PROBUFF_NET_TASK_RECEIVE_AWARD_RESPONSE>& adata)
	{
		handle_print<true>(adata);
		return true;
	}
}//namespace ngl
