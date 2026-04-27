/*
* Copyright (c) [2020-2025] NingLeixueR
* 
* Project name: ngl_server
* Project URL: https://github.com/NingLeixueR/ngl_server
* 
* This file is part of the ngl_server project and is distributed under the MIT License.
* You may use, modify, and distribute this project under the license, including commercial use,
* but you must retain the original copyright and license notice.
* 
* For license details, see the LICENSE file in the project root:
* https://github.com/NingLeixueR/ngl_server/blob/main/LICENSE
*/
// File overview: Message handler registration and dispatch for the robot actor.
#include "actor/actor_logic/actor_robot_manage/actor_robot_manage.h"
#include "actor/actor_logic/actor_robot/actor_robot.h"
#include "actor/actor_base/core/nregister.h"
#include "actor/actor_logic/nforward.h"
#include "net/udp/kcp/nkcp.h"
namespace ngl
{
	bool actor_robot::handle([[maybe_unused]] const message<np_thruput_test>& adata)
	{
		test_thruput::instance().change_rounds(adata.get_data()->m_rounds);
		return true;
	}
	bool actor_robot::handle([[maybe_unused]] const message<pbexample::PROBUFF_EXAMPLE_GUESS_NUMBER_BOMB>& adata)
	{
		handle_print(adata);
		return true;
	}
	bool actor_robot::handle([[maybe_unused]] const message<pbexample::PROBUFF_EXAMPLE_GUESS_NUMBER_ERROR>& adata)
	{
		handle_print(adata);
		return true;
	}
	bool actor_robot::handle([[maybe_unused]] const message<pbexample::PROBUFF_EXAMPLE_GUESS_NUMBER_INFO>& adata)
	{
		handle_print(adata);
		return true;
	}
	bool actor_robot::handle([[maybe_unused]] const message<pbexample::PROBUFF_EXAMPLE_PLAY_CREATE>& adata)
	{
		handle_print(adata);
		return true;
	}
	bool actor_robot::handle([[maybe_unused]] const message<pbexample::PROBUFF_EXAMPLE_PLAY_ENTER_EXAMPLE_RESPONSE>& adata)
	{
		handle_print(adata);
		return true;
	}
	bool actor_robot::handle([[maybe_unused]] const message<pbexample::PROBUFF_EXAMPLE_PLAY_JOIN_RESPONSE>& adata)
	{
		handle_print(adata);
		return true;
	}
	bool actor_robot::handle([[maybe_unused]] const message<pbexample::PROBUFF_EXAMPLE_PLAY_MATCHING>& adata)
	{
		handle_print(adata);
		return true;
	}
	bool actor_robot::handle([[maybe_unused]] const message<pbexample::PROBUFF_EXAMPLE_PLAY_MATCHING_RESULT>& adata)
	{
		handle_print(adata);
		return true;
	}
	bool actor_robot::handle([[maybe_unused]] const message<pbnet::PROBUFF_NET_BAG_SYNC_RESPONSE>& adata)
	{
		handle_print(adata);
		return true;
	}
	bool actor_robot::handle([[maybe_unused]] const message<pbnet::PROBUFF_NET_BAG_UPDATE>& adata)
	{
		handle_print(adata);
		return true;
	}
	bool actor_robot::handle([[maybe_unused]] const message<pbnet::PROBUFF_NET_CHAT_RESPONSE>& adata)
	{
		auto lrecv = adata.get_data();
		if (lrecv->mtype() == pbnet::ENUM_GET_CHAT_LIST)
		{
			char lbuff[1024] = { 0 };
			for (auto& chatitem : lrecv->mchatlist())
			{
				tools::time::time2str(lbuff, 1024, chatitem.mutc(), "%y/%m/%d %H:%M:%S");
				log_error()->print("[{}:{}:{}] {}", nguid::area(chatitem.mroleid()), chatitem.mrolename(), lbuff, chatitem.mcontent());
			}
		}
		else if (lrecv->mtype() == pbnet::ENUM_CHAT_SPEAK)
		{
			// Log_error()->print("{}", (lrecv->m_stat()? "[ successful]": "[ ] "));
		}
		else if (lrecv->mtype() == pbnet::ENUM_UPDATA_SPEAK)
		{
			char lbuff[1024] = { 0 };
			for (auto& chatitem : lrecv->mchatlist())
			{
				tools::time::time2str(lbuff, 1024, chatitem.mutc(), "%y/%m/%d %H:%M:%S");
				log_error()->print("[{}:{}:{}] {}", nguid::area(chatitem.mroleid()), chatitem.mrolename(), lbuff, chatitem.mcontent());
			}
		}
		return true;
	}
	bool actor_robot::handle([[maybe_unused]] const message<pbnet::PROBUFF_NET_DELIVER_GOODS_RECHARGE>& adata)
	{
		handle_print(adata);
		return true;
	}
	bool actor_robot::handle([[maybe_unused]] const message<pbnet::PROBUFF_NET_ENTER_LEAVE_VIEW>& adata)
	{
		handle_print(adata);
		return true;
	}
	bool actor_robot::handle([[maybe_unused]] const message<pbnet::PROBUFF_NET_ERROR>& adata)
	{
		handle_print(adata);
		return true;
	}
	bool actor_robot::handle([[maybe_unused]] const message<pbnet::PROBUFF_NET_FAMIL_CEDE_RESPONSE>& adata)
	{
		handle_print(adata);
		return true;
	}
	bool actor_robot::handle([[maybe_unused]] const message<pbnet::PROBUFF_NET_FAMIL_CHANGENAME_RESPONSE>& adata)
	{
		handle_print(adata);
		return true;
	}
	bool actor_robot::handle([[maybe_unused]] const message<pbnet::PROBUFF_NET_FAMIL_CREATE_RESPONSE>& adata)
	{
		handle_print(adata);
		return true;
	}
	bool actor_robot::handle([[maybe_unused]] const message<pbnet::PROBUFF_NET_FAMIL_INFO_RESPONSE>& adata)
	{
		handle_print(adata);
		return true;
	}
	bool actor_robot::handle([[maybe_unused]] const message<pbnet::PROBUFF_NET_FAMIL_JOIN_RESPONSE>& adata)
	{
		handle_print(adata);
		return true;
	}
	bool actor_robot::handle([[maybe_unused]] const message<pbnet::PROBUFF_NET_FAMIL_LEAVE_RESPONSE>& adata)
	{
		handle_print(adata);
		return true;
	}
	bool actor_robot::handle([[maybe_unused]] const message<pbnet::PROBUFF_NET_FAMIL_LIST_RESPONSE>& adata)
	{
		handle_print(adata);
		return true;
	}
	bool actor_robot::handle([[maybe_unused]] const message<pbnet::PROBUFF_NET_FAMIL_RATIFY_JOIN_RESPONSE>& adata)
	{
		handle_print(adata);
		return true;
	}
	bool actor_robot::handle([[maybe_unused]] const message<pbnet::PROBUFF_NET_FAMIL_SIGN_RESPONSE>& adata)
	{
		handle_print(adata);
		return true;
	}
	bool actor_robot::handle([[maybe_unused]] const message<pbnet::PROBUFF_NET_FRIEND_ADD_RESPONSE>& adata)
	{
		handle_print(adata);
		return true;
	}
	bool actor_robot::handle([[maybe_unused]] const message<pbnet::PROBUFF_NET_FRIEND_ERASE_RESPONSE>& adata)
	{
		handle_print(adata);
		return true;
	}
	bool actor_robot::handle([[maybe_unused]] const message<pbnet::PROBUFF_NET_FRIEND_RATIFY_ADD_RESPONSE>& adata)
	{
		handle_print(adata);
		return true;
	}
	bool actor_robot::handle([[maybe_unused]] const message<pbnet::PROBUFF_NET_FRIEND_RESPONSE>& adata)
	{
		handle_print(adata);
		return true;
	}
	bool actor_robot::handle([[maybe_unused]] const message<pbnet::PROBUFF_NET_GET_TIME_RESPONSE>& adata)
	{
		char lbuff[1024] = { 0 };
		tools::time::time2str(lbuff, 1024, adata.get_data()->mutc(), "%y/%m/%d %H:%M:%S");
		log_error()->print("[{}][{}]", m_data.mbrief().mbase().mname(), lbuff);
		return true;
	}
	bool actor_robot::handle([[maybe_unused]] const message<pbnet::PROBUFF_NET_KCPSESSION_RESPONSE>& adata)
	{
		auto lpram = adata.get_data();

		i32_serverid lserverid = static_cast<i32_serverid>(lpram->mserverid());
		int16_t ltcount = nnodeid::tcount(lserverid);
		int16_t ltid = static_cast<int16_t>(nnodeid::tid(lserverid));

		net_works lpstructserver;
		if (!ttab_servers::instance().get_nworks(lserverid, nconfig.area(), ENET_TCP, lpstructserver))
		{
			return false;
		}
		
		std::string lkcpsession = lpram->mkcpsession();			
		auto luport = kcp_index(lserverid, lpram->m_kcpnum());
		if (!luport.has_value())
		{
			return false;
		}
		const bool lconnected = kcp_connect(*luport
			, lpstructserver.m_ip, nkcp::instance().kcp_port(ltid, ltcount, lpram->m_kcpnum())
			, lpram->mactoridserver(), lkcpsession
		);
		return lconnected;
	}
	bool actor_robot::handle([[maybe_unused]] const message<pbnet::PROBUFF_NET_MAIL_DEL_RESPONSE>& adata)
	{
		handle_print(adata);
		return true;
	}
	bool actor_robot::handle([[maybe_unused]] const message<pbnet::PROBUFF_NET_MAIL_DRAW_RESPONSE>& adata)
	{
		handle_print(adata);
		return true;
	}
	bool actor_robot::handle([[maybe_unused]] const message<pbnet::PROBUFF_NET_MAIL_LIST_RESPONSE>& adata)
	{
		handle_print(adata);
		return true;
	}
	bool actor_robot::handle([[maybe_unused]] const message<pbnet::PROBUFF_NET_MAIL_READ_RESPONSE>& adata)
	{
		handle_print(adata);
		return true;
	}
	bool actor_robot::handle([[maybe_unused]] const message<pbnet::PROBUFF_NET_MSG_RESPONSE>& adata)
	{
		handle_print(adata);
		return true;
	}
	bool actor_robot::handle([[maybe_unused]] const message<pbnet::PROBUFF_NET_NOTICE_RESPONSE>& adata)
	{
		struct noticeitem
		{
			int64_t		m_id;			// Noticeid
			std::string m_notice;		// Content
			std::string m_starttime;	// Time
			std::string m_finishtime;	// Endtime
			DPROTOCOL(noticeitem, m_id, m_notice, m_starttime, m_finishtime)
		};
		struct notices
		{
			std::vector<noticeitem> m_notices;
			DPROTOCOL(notices, m_notices)
		};
		ncjson ljson;
		notices lnotices;
		char lbuffstart[1024] = { 0 };
		char lbufffinish[1024] = { 0 };
		for (const auto& lnotice : adata.get_data()->mnotices())
		{
			tools::time::time2str(lbuffstart, 1024, lnotice.mstarttime(), "%Y/%m/%d %H:%M:%S");
			tools::time::time2str(lbufffinish, 1024, lnotice.mfinishtime(), "%Y/%m/%d %H:%M:%S");
			lnotices.m_notices.emplace_back(
				noticeitem
				{
					.m_id = lnotice.mid(),
					.m_notice = lnotice.mnotice(),
					.m_starttime = lbuffstart,
					.m_finishtime = lbufffinish,
				}
			);
		}
		njson::push(ljson, { tools::type_name<notices>().c_str() }, lnotices);
		std::string lstr = ljson.str();
		std::string lstrasscii;
		ngl::tools::to_asscii(lstr, lstrasscii);
		log_error()->print("[{}:{}] {}",area(), m_data.mbrief().mbase().mname(), lstrasscii);
		return true;
	}
	bool actor_robot::handle([[maybe_unused]] const message<pbnet::PROBUFF_NET_RANKLIST_RESPONSE>& adata)
	{
		handle_print(adata);
		return true;
	}
	bool actor_robot::handle([[maybe_unused]] const message<pbnet::PROBUFF_NET_RECHARGE_RESPONSE>& adata)
	{
		handle_print(adata);
		return true;
	}
	bool actor_robot::handle([[maybe_unused]] const message<pbnet::PROBUFF_NET_REWARD_ITEM_RESPONSE>& adata)
	{
		handle_print(adata);
		return true;
	}
	bool actor_robot::handle([[maybe_unused]] const message<pbnet::PROBUFF_NET_ROLESTAT>& adata)
	{
		handle_print(adata);
		return true;
	}
	bool actor_robot::handle([[maybe_unused]] const message<pbnet::PROBUFF_NET_ROLE_SYNC_RESPONSE>& adata)
	{
		log_error()->print("[LOGIC_ROLE_SYNC:{}:{}]", adata.get_data()->mbrief().mbase().mname(), adata.get_data()->mbrief().mbase().mlv());
		m_data = *adata.get_data();
		handle_print(adata);

		if (!m_firstsync)
		{
			m_firstsync = true;
		}
		if (!m_bt.empty())
		{
			m_bt.tick();
		}
		return true;
	}
	bool actor_robot::handle([[maybe_unused]] const message<pbnet::PROBUFF_NET_SWITCH_LINE_RESPONSE>& adata)
	{
		handle_print(adata);
		return true;
	}
	bool actor_robot::handle([[maybe_unused]] const message<pbnet::PROBUFF_NET_SYNC_ATTRIBUTE>& adata)
	{
		handle_print(adata);
		return true;
	}
	bool actor_robot::handle([[maybe_unused]] const message<pbnet::PROBUFF_NET_SYNC_POSITION>& adata)
	{
		handle_print(adata);
		return true;
	}
	bool actor_robot::handle([[maybe_unused]] const message<pbnet::PROBUFF_NET_SYNC_UNIT>& adata)
	{
		handle_print(adata);
		return true;
	}
	bool actor_robot::handle([[maybe_unused]] const message<pbnet::PROBUFF_NET_TASK_RECEIVE_AWARD_RESPONSE>& adata)
	{
		handle_print(adata);
		return true;
	}
}//namespace ngl