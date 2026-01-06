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
#include "actor_family.h"
namespace ngl
{
	bool actor_family::handle(const message<mforward<np_gm>>& adata)
	{
		ncjson lojson(adata.get_data()->data()->m_json.c_str());
		std::string loperator;
		if (!njson::pop(lojson.json(), "operator", loperator))
		{
			return true;
		}
		if (handle_cmd::empty())
		{
			handle_cmd::add("change_familyname") = [this](int id, ncjson& aos)
				{
					gcmd<bool> pro(id, "change_familyname", false);
					struct gm_changename
					{
						int64_t m_familid = 0;
						std::string m_familname;
						dprotocol(gm_changename, m_familid, m_familname)
					};
					gm_changename recv;
					if (!njson::pop(aos.json(), "data", recv))
					{
						return;
					}
					pro.m_data = true;
					m_family.change_familyname(-1, recv.m_familid, recv.m_familname);
				};

			handle_cmd::add("get_family") = [this](int id, ncjson& aos)
				{
					gcmd<std::string> pro(id, "get_family");
					int64_t familid = 0;
					if (!njson::pop(aos.json(), "data", familid))
					{
						return;
					}
					pbnet::PROBUFF_NET_FAMIL_LIST_RESPONSE profamilylist;
					if (m_family.get_familylist(familid, profamilylist))
					{
						tools::proto2json(profamilylist, pro.m_data);
					}
				};
		}

		if (handle_cmd::function(loperator, (int32_t)adata.get_data()->identifier(), lojson) == false)
		{
			log_error()->print("GM actor_mail operator[{}] ERROR", loperator);
		}
		return true;
	}
	bool actor_family::handle(const message<mforward<pbnet::PROBUFF_NET_FAMIL_CEDE>>& adata)
	{
		i64_actorid lroleid = adata.get_data()->identifier();
		i64_actorid lcederoleid = adata.get_data()->data()->mroleid();
		pbnet::PROBUFF_NET_FAMIL_CEDE_RESPONSE pro;
		int32_t lstat = m_family.cede_family(lroleid, lcederoleid);
		pro.set_mstat(lstat);
		send_client(lroleid, pro);
		return true;
	}
	bool actor_family::handle(const message<mforward<pbnet::PROBUFF_NET_FAMIL_CHANGENAME>>& adata)
	{
		i64_actorid lroleid = adata.get_data()->identifier();
		i64_actorid familid = adata.get_data()->data()->mfamilid();

		pbnet::PROBUFF_NET_FAMIL_CHANGENAME_RESPONSE pro;
		int32_t lstat = m_family.change_familyname(lroleid, familid, adata.get_data()->data()->mname());
		pro.set_mstat(lstat);
		send_client(lroleid, pro);
		return true;
	}
	bool actor_family::handle(const message<mforward<pbnet::PROBUFF_NET_FAMIL_CREATE>>& adata)
	{
		i64_actorid lroleid = adata.get_data()->identifier();
		pbnet::PROBUFF_NET_FAMIL_CREATE_RESPONSE pro;
		int32_t lstat = m_family.create_family(lroleid, adata.get_data()->data()->mname());
		pro.set_mstat(lstat);
		send_client(lroleid, pro);
		return true;
	}
	bool actor_family::handle(const message<mforward<pbnet::PROBUFF_NET_FAMIL_INFO>>& adata)
	{
		i64_actorid lroleid = adata.get_data()->identifier();
		m_family.sync_family(lroleid);
		return true;
	}
	bool actor_family::handle(const message<mforward<pbnet::PROBUFF_NET_FAMIL_JOIN>>& adata)
	{
		i64_actorid lroleid = adata.get_data()->identifier();
		i64_actorid familid = adata.get_data()->data()->mfamilid();

		pbnet::PROBUFF_NET_FAMIL_JOIN_RESPONSE pro;
		int32_t lstat = 0;
		if (adata.get_data()->data()->mapply())
		{
			lstat = m_family.join_family(lroleid, familid);
		}
		else
		{
			lstat = m_family.cancel_join_family(lroleid, familid);
		}

		pro.set_mstat(lstat);
		send_client(lroleid, pro);
		return true;
	}
	bool actor_family::handle(const message<mforward<pbnet::PROBUFF_NET_FAMIL_LEAVE>>& adata)
	{
		i64_actorid lroleid = adata.get_data()->identifier();
		i64_actorid familid = adata.get_data()->data()->mfamilid();

		pbnet::PROBUFF_NET_FAMIL_LEAVE_RESPONSE pro;
		int32_t lstat = m_family.leave_family(lroleid, familid);
		pro.set_mstat(lstat);
		send_client(lroleid, pro);
		return true;
	}
	bool actor_family::handle(const message<mforward<pbnet::PROBUFF_NET_FAMIL_LIST>>& adata)
	{
		m_family.sync_familylist(adata.get_data()->identifier(), adata.get_data()->data()->mfamilid());
		return true;
	}
	bool actor_family::handle(const message<mforward<pbnet::PROBUFF_NET_FAMIL_RATIFY_JOIN>>& adata)
	{
		i64_actorid lroleid = adata.get_data()->identifier();
		i64_actorid ljoinroleid = adata.get_data()->data()->mroleid();

		pbnet::PROBUFF_NET_FAMIL_RATIFY_JOIN_RESPONSE pro;
		int32_t lstat = m_family.ratify_join_family(lroleid, ljoinroleid, adata.get_data()->data()->mratify());
		pro.set_mstat(lstat);
		send_client(lroleid, pro);
		return true;
	}
	bool actor_family::handle(const message<mforward<pbnet::PROBUFF_NET_FAMIL_SIGN>>& adata)
	{
		i64_actorid lroleid = adata.get_data()->identifier();
		i64_actorid familid = adata.get_data()->data()->mfamilid();

		pbnet::PROBUFF_NET_FAMIL_SIGN_RESPONSE pro;
		int32_t lstat = m_family.sign_family(lroleid, familid);
		pro.set_mstat(lstat);
		send_client(lroleid, pro);
		return true;
	}
	bool actor_family::handle(const message<np_eevents_logic_rolelogin>& adata)
	{
		std::vector<i64_actorid> lfamilyers;
		if (m_family.get_familyers(adata.get_data()->m_actorid, lfamilyers))
		{
			pbnet::PROBUFF_NET_ROLESTAT pro;
			pro.set_mstat(pbnet::PROBUFF_NET_ROLESTAT::online);
			pro.set_mlogicstat(pbnet::PROBUFF_NET_ROLESTAT::familyer);
			pro.set_mroleid(adata.get_data()->m_actorid);
			send_client(lfamilyers, pro);
		}
		return true;
	}
	bool actor_family::handle(const message<np_eevents_logic_roleoffline>& adata)
	{
		std::vector<i64_actorid> lfamilyers;
		if (m_family.get_familyers(adata.get_data()->m_actorid, lfamilyers))
		{
			pbnet::PROBUFF_NET_ROLESTAT pro;
			pro.set_mstat(pbnet::PROBUFF_NET_ROLESTAT::offline);
			pro.set_mlogicstat(pbnet::PROBUFF_NET_ROLESTAT::familyer);
			pro.set_mroleid(adata.get_data()->m_actorid);
			send_client(lfamilyers, pro);
		}
		return true;
	}
}//namespace ngl