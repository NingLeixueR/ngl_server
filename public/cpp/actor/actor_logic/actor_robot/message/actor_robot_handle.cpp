/*
* Copyright (c) [2020-2025] NingLeixueR
* 
* 项目名称：ngl_server
* 项目地址：https://github.com/NingLeixueR/ngl_server
* 
* 本文件是 ngl_server 项目的一部分，遵循 MIT 开源协议发布。
* 您可以按照协议规定自由使用、修改和分发本项目，包括商业用途，
* 但需保留原始版权和许可声明。
* 
* 许可详情参见项目根目录下的 LICENSE 文件：
* https://github.com/NingLeixueR/ngl_server/blob/main/LICENSE
*/
#include "actor_robot_manage.h"
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
		handle_print(adata);
		return true;
	}
	bool actor_robot::handle(const message<pbexample::PROBUFF_EXAMPLE_GUESS_NUMBER_ERROR>& adata)
	{
		handle_print(adata);
		return true;
	}
	bool actor_robot::handle(const message<pbexample::PROBUFF_EXAMPLE_GUESS_NUMBER_INFO>& adata)
	{
		handle_print(adata);
		return true;
	}
	bool actor_robot::handle(const message<pbexample::PROBUFF_EXAMPLE_PLAY_CREATE>& adata)
	{
		handle_print(adata);
		return true;
	}
	bool actor_robot::handle(const message<pbexample::PROBUFF_EXAMPLE_PLAY_ENTER_EXAMPLE_RESPONSE>& adata)
	{
		handle_print(adata);
		return true;
	}
	bool actor_robot::handle(const message<pbexample::PROBUFF_EXAMPLE_PLAY_JOIN_RESPONSE>& adata)
	{
		handle_print(adata);
		return true;
	}
	bool actor_robot::handle(const message<pbexample::PROBUFF_EXAMPLE_PLAY_MATCHING>& adata)
	{
		handle_print(adata);
		return true;
	}
	bool actor_robot::handle(const message<pbexample::PROBUFF_EXAMPLE_PLAY_MATCHING_RESULT>& adata)
	{
		handle_print(adata);
		return true;
	}
	bool actor_robot::handle(const message<pbnet::PROBUFF_NET_BAG_SYNC_RESPONSE>& adata)
	{
		handle_print(adata);
		return true;
	}
	bool actor_robot::handle(const message<pbnet::PROBUFF_NET_BAG_UPDATE>& adata)
	{
		handle_print(adata);
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
		handle_print(adata);
		return true;
	}
	bool actor_robot::handle(const message<pbnet::PROBUFF_NET_ENTER_LEAVE_VIEW>& adata)
	{
		handle_print(adata);
		return true;
	}
	bool actor_robot::handle(const message<pbnet::PROBUFF_NET_ERROR>& adata)
	{
		handle_print(adata);
		return true;
	}
	bool actor_robot::handle(const message<pbnet::PROBUFF_NET_FAMIL_CEDE_RESPONSE>& adata)
	{
		handle_print(adata);
		return true;
	}
	bool actor_robot::handle(const message<pbnet::PROBUFF_NET_FAMIL_CHANGENAME_RESPONSE>& adata)
	{
		handle_print(adata);
		return true;
	}
	bool actor_robot::handle(const message<pbnet::PROBUFF_NET_FAMIL_CREATE_RESPONSE>& adata)
	{
		handle_print(adata);
		return true;
	}
	bool actor_robot::handle(const message<pbnet::PROBUFF_NET_FAMIL_INFO_RESPONSE>& adata)
	{
		handle_print(adata);
		return true;
	}
	bool actor_robot::handle(const message<pbnet::PROBUFF_NET_FAMIL_JOIN_RESPONSE>& adata)
	{
		handle_print(adata);
		return true;
	}
	bool actor_robot::handle(const message<pbnet::PROBUFF_NET_FAMIL_LEAVE_RESPONSE>& adata)
	{
		handle_print(adata);
		return true;
	}
	bool actor_robot::handle(const message<pbnet::PROBUFF_NET_FAMIL_LIST_RESPONSE>& adata)
	{
		handle_print(adata);
		return true;
	}
	bool actor_robot::handle(const message<pbnet::PROBUFF_NET_FAMIL_RATIFY_JOIN_RESPONSE>& adata)
	{
		handle_print(adata);
		return true;
	}
	bool actor_robot::handle(const message<pbnet::PROBUFF_NET_FAMIL_SIGN_RESPONSE>& adata)
	{
		handle_print(adata);
		return true;
	}
	bool actor_robot::handle(const message<pbnet::PROBUFF_NET_FRIEND_ADD_RESPONSE>& adata)
	{
		handle_print(adata);
		return true;
	}
	bool actor_robot::handle(const message<pbnet::PROBUFF_NET_FRIEND_ERASE_RESPONSE>& adata)
	{
		handle_print(adata);
		return true;
	}
	bool actor_robot::handle(const message<pbnet::PROBUFF_NET_FRIEND_RATIFY_ADD_RESPONSE>& adata)
	{
		handle_print(adata);
		return true;
	}
	bool actor_robot::handle(const message<pbnet::PROBUFF_NET_FRIEND_RESPONSE>& adata)
	{
		handle_print(adata);
		return true;
	}
	bool actor_robot::handle(const message<pbnet::PROBUFF_NET_GET_TIME_RESPONSE>& adata)
	{
		char lbuff[1024] = { 0 };
		ngl::localtime::time2str(lbuff, 1024, adata.get_data()->mutc(), "%y/%m/%d %H:%M:%S");
		log_error()->print("[{}][{}]", m_data.mbrief().mbase().mname(), lbuff);
		return true;
	}
	bool actor_robot::handle(const message<pbnet::PROBUFF_NET_KCPSESSION_RESPONSE>& adata)
	{
		auto lpram = adata.get_data();
		const tab_servers* tab = ttab_servers::instance().tab();

		net_works lpstructserver;
		if (!ttab_servers::instance().get_nworks("gateway", nconfig::area(), nnodeid::tcount(lpram->mserverid()), ENET_KCP, lpstructserver))
		{
			return false;
		}
		
		std::string lkcpsession = lpram->mkcpsession();	
		return connect_kcp(
			kcpindex(lpram->mserverid(), lpram->m_kcpnum())
			, lpstructserver.m_ip, nets::kcp_port(lpstructserver, nnodeid::tcount(lpram->mserverid()), lpram->m_kcpnum())
			, nguid::make()/*nguid::make_type(id_guid(), ACTOR_ROLE)*/, lkcpsession
		);
	}
	bool actor_robot::handle(const message<pbnet::PROBUFF_NET_MAIL_DEL_RESPONSE>& adata)
	{
		handle_print(adata);
		return true;
	}
	bool actor_robot::handle(const message<pbnet::PROBUFF_NET_MAIL_DRAW_RESPONSE>& adata)
	{
		handle_print(adata);
		return true;
	}
	bool actor_robot::handle(const message<pbnet::PROBUFF_NET_MAIL_LIST_RESPONSE>& adata)
	{
		handle_print(adata);
		return true;
	}
	bool actor_robot::handle(const message<pbnet::PROBUFF_NET_MAIL_READ_RESPONSE>& adata)
	{
		handle_print(adata);
		return true;
	}
	bool actor_robot::handle(const message<pbnet::PROBUFF_NET_MSG_RESPONSE>& adata)
	{
		handle_print(adata);
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
			dprotocol(noticeitem, m_id, m_notice, m_starttime, m_finishtime)
		};
		struct notices
		{
			std::vector<noticeitem> m_notices;
			dprotocol(notices, m_notices)
		};
		njson_write ljson;
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
		njson::write(ljson, tools::type_name<notices>().c_str(), lnotices);
		std::string lstr = ljson.get();
		std::string lstrasscii;
		ngl::tools::to_asscii(lstr, lstrasscii);
		log_error()->print(
			"[{}:{}] {}",
			area(), m_data.mbrief().mbase().mname(), lstrasscii
		);
		return true;
	}
	bool actor_robot::handle(const message<pbnet::PROBUFF_NET_RANKLIST_RESPONSE>& adata)
	{
		handle_print(adata);
		return true;
	}
	bool actor_robot::handle(const message<pbnet::PROBUFF_NET_RECHARGE_RESPONSE>& adata)
	{
		handle_print(adata);
		return true;
	}
	bool actor_robot::handle(const message<pbnet::PROBUFF_NET_REWARD_ITEM_RESPONSE>& adata)
	{
		handle_print(adata);
		return true;
	}
	bool actor_robot::handle(const message<pbnet::PROBUFF_NET_ROLESTAT>& adata)
	{
		handle_print(adata);
		return true;
	}
	bool actor_robot::handle(const message<pbnet::PROBUFF_NET_ROLE_SYNC_RESPONSE>& adata)
	{
		log_error()->print("[LOGIC_ROLE_SYNC:{}:{}]"
			, adata.get_data()->mbrief().mbase().mname()
			, adata.get_data()->mbrief().mbase().mlv()
		);
		m_data = *adata.get_data();
		handle_print(adata);

		std::string lcmd = "kcp 1";
		std::vector<std::string> lvec;
		if (ngl::tools::splite(lcmd.c_str(), " ", lvec) == false)
		{
			return false;
		}
		ngl::actor_robot_manage::parse_command(lvec);

		return true;
	}
	bool actor_robot::handle(const message<pbnet::PROBUFF_NET_SWITCH_LINE_RESPONSE>& adata)
	{
		handle_print(adata);
		return true;
	}
	bool actor_robot::handle(const message<pbnet::PROBUFF_NET_SYNC_ATTRIBUTE>& adata)
	{
		handle_print(adata);
		return true;
	}
	bool actor_robot::handle(const message<pbnet::PROBUFF_NET_SYNC_POSITION>& adata)
	{
		handle_print(adata);
		return true;
	}
	bool actor_robot::handle(const message<pbnet::PROBUFF_NET_SYNC_UNIT>& adata)
	{
		handle_print(adata);
		return true;
	}
	bool actor_robot::handle(const message<pbnet::PROBUFF_NET_TASK_RECEIVE_AWARD_RESPONSE>& adata)
	{
		handle_print(adata);
		return true;
	}
}//namespace ngl
