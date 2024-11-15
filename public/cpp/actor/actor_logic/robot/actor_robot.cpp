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

		register_handle_proto<actor_robot>::func<
			pbnet::PROBUFF_NET_KCPSESSION_RESPONSE
		>(false);
	}

	const char* actor_robot::kcp_session()
	{
		return m_kcpsessionmd5.c_str();
	}

	bool actor_robot::handle(const message<pbnet::PROBUFF_NET_ROLE_SYNC_RESPONSE>& adata)
	{
		Try
		{
			
			log_error()->print("[LOGIC_ROLE_SYNC:{}:{}]"
				, adata.get_data()->m_role().m_base().m_name()
				,  adata.get_data()->m_role().m_base().m_lv()
			);
			m_data = *adata.get_data();
		}Catch
		return true;
	}

	bool actor_robot::handle(const message<pbnet::PROBUFF_NET_GET_TIME_RESPONSE>& adata)
	{
		char lbuff[1024] = { 0 };
		ngl::localtime::time2str(lbuff, 1024, adata.get_data()->m_utc(), "%y/%m/%d %H:%M:%S");
		log_error()->print("[{}][{}]", m_data.m_role().m_base().m_name(), lbuff);
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
	
	bool actor_robot::handle(const message<pbnet::PROBUFF_NET_SWITCH_LINE_RESPONSE>& adata)
	{
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
			area(), m_data.m_role().m_base().m_name(), lstrasscii
		);
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
		return true;
	}

	bool actor_robot::handle(const message<pbnet::PROBUFF_NET_MAIL_DRAW_RESPONSE>& adata)
	{
		return true;
	}

	bool actor_robot::handle(const message<pbnet::PROBUFF_NET_MAIL_DEL_RESPONSE>& adata)
	{
		return true;
	}

	bool actor_robot::handle(const message<pbnet::PROBUFF_NET_DELIVER_GOODS_RECHARGE>& adata)
	{
		log_error()->print("发货成功！！！");
		return true;
	}

	bool actor_robot::handle(const message<pbnet::PROBUFF_NET_ERROR>& adata)
	{
		log_error()->print("[{}][{}]"
			, nguid::make_type(id_guid(), ACTOR_ROLE)
			, adata.get_data()->m_errmessage()
		);
		return true;
	}

	bool actor_robot::handle(const message<pbnet::PROBUFF_NET_KCPSESSION_RESPONSE>& adata)
	{
		const tab_servers* tab = ttab_servers::tab();
		m_kcpsessionmd5 = adata.get_data()->m_kcpsession();

		const tab_servers* tabgame = ttab_servers::tab("game", tab->m_area, 1);
		net_works const*  lpworks = ttab_servers::nworks(ENET_KCP, tabgame);
		if (lpworks == nullptr)
			return false;
		return connect_kcp(m_kcp, lpworks->m_ip, lpworks->m_port);
	}

	bool actor_robot::handle(const message<pbnet::PROBUFF_NET_CREATE_FAMIL_RESPONSE>& adata)
	{
		auto lstream = log_error();
		(*lstream) << 
			std::format(
				"创建军团{}！err={}", 
				adata.get_data()->m_stat() == 0 ? "成功" : "失败", 
				adata.get_data()->m_stat()
			);
		lstream->print("");
		return true;
	}

	bool actor_robot::handle(const message<pbnet::PROBUFF_NET_JOIN_FAMIL_RESPONSE>& adata)
	{
		auto lstream = log_error();
		(*lstream) <<
			std::format(
				"加入军团{}！err={}",
				adata.get_data()->m_stat() == 0 ? "成功" : "失败",
				adata.get_data()->m_stat()
			);
		lstream->print("");
		return true;
	}


	bool actor_robot::handle(const message<pbnet::PROBUFF_NET_RATIFY_JOIN_FAMIL_RESPONSE>& adata)
	{
		auto lstream = log_error();
		(*lstream) <<
			std::format(
				"审核玩家加入军团{}！err = {}",
				adata.get_data()->m_stat() == 0 ? "成功" : "失败",
				adata.get_data()->m_stat()
			);
		lstream->print("");
		return true;
	}

	bool actor_robot::handle(const message<pbnet::PROBUFF_NET_CEDE_FAMIL_RESPONSE>& adata)
	{
		auto lstream = log_error();
		(*lstream) <<
			std::format(
				"转让军团长{}！err = {}",
				adata.get_data()->m_stat() == 0 ? "成功" : "失败",
				adata.get_data()->m_stat()
			);
		lstream->print("");
		return true;
	}

	bool actor_robot::handle(const message<pbnet::PROBUFF_NET_LEAVE_FAMIL_RESPONSE>& adata)
	{
		auto lstream = log_error();
		(*lstream) <<
			std::format(
				"离开军团{}！err={}",
				adata.get_data()->m_stat() == 0 ? "成功" : "失败",
				adata.get_data()->m_stat()
			);
		lstream->print("");
		return true;
	}

	bool actor_robot::handle(const message<pbnet::PROBUFF_NET_FAMIL_LIST_RESPONSE>& adata)
	{
		const pbnet::PROBUFF_NET_FAMIL_LIST_RESPONSE* lpdata = adata.get_data();
		auto lstream = log_error();
		(*lstream) << std::endl;
		(*lstream) << "##############famil list start##############" << std::endl;
		for (const auto& item : lpdata->m_family())
		{
			(*lstream) << std::format(
				"id:{} name:{} lv:{} exp:{} createutc:{} leader:{}",
				item.m_id(),
				item.m_name(),
				item.m_lv(),
				item.m_exp(),
				item.m_createutc(),
				item.m_leader()
			) << std::endl;
		}
		(*lstream) << "##############famil list finish##############" << std::endl;
		lstream->print("");
		return true;
	}

	bool actor_robot::handle(const message<pbnet::PROBUFF_NET_CHANGE_FAMILNAME_RESPONSE>& adata)
	{
		auto lstream = log_error();
		(*lstream) <<
			std::format(
				"修改军团名称{}！err={}",
				adata.get_data()->m_stat() == 0 ? "成功" : "失败",
				adata.get_data()->m_stat()
			);
		lstream->print("");
		return true;
	}

	bool actor_robot::handle(const message<pbnet::PROBUFF_NET_FAMILSIGN_RESPONSE>&)
	{
		return true;
	}

	bool actor_robot::handle(const message<pbnet::PROBUFF_NET_REWARD_ITEM_RESPONSE>&)
	{
		return true;
	}

	bool actor_robot::handle(const message<pbnet::PROBUFF_NET_RANKLIST_RESPONSE>& adata)
	{
		auto recv = adata.get_data();
		auto lstream = log_error();
		(*lstream) << std::format("#####ranklist type={}#####", (int)(recv->m_type())) << std::endl;
		std::ranges::for_each(recv->m_items(), [&recv,&lstream](const auto& item)
			{
				if (recv->m_type() == pbdb::lv)
				{
					(*lstream) << std::format("roleid:{} name:{} lv:{}", item.m_id(), item.m_name(), item.m_lv()) << std::endl;
				}
			});
		lstream->print("");
		return true;
	}

	bool actor_robot::handle(const message<pbnet::PROBUFF_NET_FRIEND_RESPONSE>&)
	{
		log_error()->print("pbnet::PROBUFF_NET_FRIEND_RESPONSE");
		return true;
	}

	bool actor_robot::handle(const message<pbnet::PROBUFF_NET_ADDFRIEND_RESPONSE>&)
	{
		return true;
	}

	bool actor_robot::handle(const message<pbnet::PROBUFF_NET_RATIFY_ADDFRIEND_RESPONSE>&)
	{
		return true;
	}

	bool actor_robot::handle(const message<pbnet::PROBUFF_NET_ERASEFRIEND_RESPONSE>&)
	{
		return true;
	}

	bool actor_robot::handle(const message<pbnet::PROBUFF_NET_FRIEND_ROLELOGIN>& adata)
	{
		log_error()->print("pbnet::PROBUFF_NET_FRIEND_ROLELOGIN ####### {}", nguid(adata.get_data()->m_friedid()));
		return true;
	}

	void actor_manage_robot::nregister()
	{
		register_handle_custom<actor_manage_robot>::func<
			np_robot_pram
		>(false);
		
		register_handle_proto<actor_manage_robot>::func<
			pbnet::PROBUFF_NET_ACOUNT_LOGIN_RESPONSE
		>(false);
	}
}//namespace ngl