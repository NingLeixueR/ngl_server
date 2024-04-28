#include "actor_robot.h"
#include "nregister.h"
#include "nforward.h"

namespace ngl
{
	actor_robot::actor_robot(i16_area aarea, i32_actordataid arobotid, void*) :
		actor(
			actorparm
			{
				.m_parm
				{
					.m_type = ACTOR_ROBOT,
					.m_area = aarea,
					.m_id = arobotid,
					.m_manage_dbclient = true
				},
				.m_weight = 0x7fffffff,
			})
	{
		assert(aarea == ttab_servers::tab()->m_area);
	}

	void actor_robot::nregister()
	{
		nforward::g2c();
		register_actor<EPROTOCOL_TYPE_PROTOCOLBUFF, actor_robot>(
			false
			, dregister_fun_handle(actor_robot, pbnet::PROBUFF_NET_KCPSESSION_RESPONSE)
		);
	}

	bool actor_robot::handle(message<pbnet::PROBUFF_NET_ROLE_SYNC_RESPONSE>& adata)
	{
		Try
		{
			LogLocalError("[LOGIC_ROLE_SYNC:%:%]"
				, adata.m_data->m_role().m_base().m_name()
				,  adata.m_data->m_role().m_base().m_lv()
			);
			m_data = *adata.m_data;
		}Catch;
		return true;
	}

	bool actor_robot::handle(message<pbnet::PROBUFF_NET_GET_TIME_RESPONSE>& adata)
	{
		char lbuff[1024] = { 0 };
		ngl::localtime::time2str(lbuff, 1024, adata.m_data->m_utc(), "%y/%m/%d %H:%M:%S");
		LogLocalError("[%][%]", m_data.m_role().m_base().m_name(), lbuff);
		return true;
	}

	bool actor_robot::handle(message<pbnet::PROBUFF_NET_CHAT_RESPONSE>& adata)
	{
		auto lrecv = adata.m_data;
		if (lrecv->m_type() == pbnet::get_chat_list)
		{
			char lbuff[1024] = { 0 };
			for (auto& item : lrecv->m_chatlist())
			{
				ngl::localtime::time2str(lbuff, 1024, item.m_utc(), "%y/%m/%d %H:%M:%S");
				LogLocalError("[%:%:%] %", nguid::area(item.m_roleid()), item.m_rolename(), lbuff, item.m_content());
			}
		}
		else if (lrecv->m_type() == pbnet::chat_speak)
		{
			//LogLocalError("%", (lrecv->m_stat() ? "[发言成功]" : "[发言失败] "));
		}
		else if (lrecv->m_type() == pbnet::updata_speck)
		{
			char lbuff[1024] = { 0 };
			for (auto& item : lrecv->m_chatlist())
			{
				ngl::localtime::time2str(lbuff, 1024, item.m_utc(), "%y/%m/%d %H:%M:%S");
				LogLocalError("[%:%:%] %", nguid::area(item.m_roleid()), item.m_rolename(), lbuff, item.m_content());
			}
		}
		return true;
	}
	
	bool actor_robot::handle(message<pbnet::PROBUFF_NET_SWITCH_LINE_RESPONSE>& adata)
	{
		return true;
	}

	bool actor_robot::handle(message<pbnet::PROBUFF_NET_NOTICE_RESPONSE>& adata)
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
		ijson ljson;
		notices lnotices;
		char lbuffstart[1024] = { 0 };
		char lbufffinish[1024] = { 0 };
		for (auto& item : adata.m_data->m_notices())
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
		ngl::conversion::to_asscii(lstr, lstrasscii);
		LogLocalError("[%:%] %", area(), m_data.m_role().m_base().m_name(), lstrasscii);
		return true;
	}

	bool actor_robot::handle(message<pbnet::PROBUFF_NET_MAIL_LIST_RESPONSE>& adata)
	{
		pbnet::PROBUFF_NET_MAIL_LIST_RESPONSE& pro = *adata.m_data;
		char lbuff[1024] = { 0 };
		LogLocalStreamError(lstream);
		for (const auto& [_id, _mail] : pro.m_mail())
		{
			lstream << "[####################################################################]" << std::endl;
			lstream << "[mailid:	" << _mail.m_id() << "]" << std::endl;
			std::string lcontent;
			conversion::to_asscii(_mail.m_content(), lcontent);
			lstream << "[content:	" << lcontent << "]" << std::endl;
			ngl::localtime::time2str(lbuff, 1024, _mail.m_createutc(), "%y/%m/%d %H:%M:%S");
			lstream << "[createutc:	" << lbuff << "]" << std::endl;
			lstream << "[read:		" << (_mail.m_read() ? "yes" : "no") << "]" << std::endl;
			lstream << "[draw:		" << (_mail.m_draw() ? "yes" : "no") << "]" << std::endl;
			lstream << "[####################################################################]" << std::endl;
		}
		lstream.print();
		return true;
	}

	bool actor_robot::handle(message<pbnet::PROBUFF_NET_MAIL_READ_RESPONSE>& adata)
	{
		return true;
	}

	bool actor_robot::handle(message<pbnet::PROBUFF_NET_MAIL_DRAW_RESPONSE>& adata)
	{
		return true;
	}

	bool actor_robot::handle(message<pbnet::PROBUFF_NET_MAIL_DEL_RESPONSE>& adata)
	{
		return true;
	}

	bool actor_robot::handle(message<pbnet::PROBUFF_NET_DELIVER_GOODS_RECHARGE>& adata)
	{
		LogLocalError("发货成功！！！");
		return true;
	}

	bool actor_robot::handle(message<pbnet::PROBUFF_NET_ERROR_RESPONSE>& adata)
	{
		LogLocalError("[%][%][%]"
			, nguid::make_type(id_guid(), ACTOR_ROLE)
			, adata.m_data->m_errnum()
			, adata.m_data->m_errmessage()
		);
		return true;
	}

	bool actor_robot::handle(message<pbnet::PROBUFF_NET_KCPSESSION_RESPONSE>& adata)
	{
		tab_servers* tab = ttab_servers::tab();
		m_kcpsessionmd5 = adata.m_data->m_kcpsession();

		tab_servers* tabgame = ttab_servers::tab("game", tab->m_area, 1);
		net_works const*  lpworks = ttab_servers::nworks(ENET_KCP, tabgame);
		if (lpworks == nullptr)
			return false;
		return connect_kcp(m_kcp, lpworks->m_ip, lpworks->m_port);
	}

	void actor_manage_robot::nregister()
	{
		register_actor<EPROTOCOL_TYPE_CUSTOM, actor_manage_robot>(
			false
			, dregister_fun_handle(actor_manage_robot, np_robot_pram)
		);

		register_actor<EPROTOCOL_TYPE_PROTOCOLBUFF, actor_manage_robot>(
			false
			, dregister_fun_handle(actor_manage_robot, pbnet::PROBUFF_NET_ACOUNT_LOGIN_RESPONSE)
		);
	}
}//namespace ngl