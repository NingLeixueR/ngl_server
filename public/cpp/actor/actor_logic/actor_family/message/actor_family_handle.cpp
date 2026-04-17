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
// File overview: Message handler registration and dispatch for the family actor.
#include "actor/actor_logic/actor_family/actor_family.h"
namespace ngl
{
	bool actor_family::handle([[maybe_unused]] const message<mforward<np_gm>>& adata)
	{
		const auto* lparm = adata.get_data();
		const auto* lrecv = lparm->data();
		ncjson lojson(lrecv->m_json.c_str());
		std::string loperator;
		if (!njson::pop(lojson, { "operator" }, loperator))
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
						DPROTOCOL(gm_changename, m_familid, m_familname)
					};
					gm_changename recv;
					if (!njson::pop(aos, { "data" }, recv))
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
					if (!njson::pop(aos, { "data" }, familid))
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

		if (handle_cmd::function(loperator, (int32_t)lparm->identifier(), lojson) == false)
		{
			log_error()->print("GM actor_mail operator[{}] ERROR", loperator);
		}
		return true;
	}
	bool actor_family::handle([[maybe_unused]] const message<mforward<pbnet::PROBUFF_NET_FAMIL_CEDE>>& adata)
	{
		const auto* lparm = adata.get_data();
		const auto* lrecv = lparm->data();
		i64_actorid lroleid = lparm->identifier();
		i64_actorid lcederoleid = lrecv->mroleid();
		pbnet::PROBUFF_NET_FAMIL_CEDE_RESPONSE pro;
		int32_t lstat = m_family.cede_family(lroleid, lcederoleid);
		pro.set_mstat(lstat);
		send_client(lroleid, pro);
		return true;
	}
	bool actor_family::handle([[maybe_unused]] const message<mforward<pbnet::PROBUFF_NET_FAMIL_CHANGENAME>>& adata)
	{
		const auto* lparm = adata.get_data();
		const auto* lrecv = lparm->data();
		i64_actorid lroleid = lparm->identifier();
		i64_actorid familid = lrecv->mfamilid();

		pbnet::PROBUFF_NET_FAMIL_CHANGENAME_RESPONSE pro;
		int32_t lstat = m_family.change_familyname(lroleid, familid, lrecv->mname());
		pro.set_mstat(lstat);
		send_client(lroleid, pro);
		return true;
	}
	bool actor_family::handle([[maybe_unused]] const message<mforward<pbnet::PROBUFF_NET_FAMIL_CREATE>>& adata)
	{
		const auto* lparm = adata.get_data();
		const auto* lrecv = lparm->data();
		i64_actorid lroleid = lparm->identifier();
		pbnet::PROBUFF_NET_FAMIL_CREATE_RESPONSE pro;
		int32_t lstat = m_family.create_family(lroleid, lrecv->mname());
		pro.set_mstat(lstat);
		send_client(lroleid, pro);
		return true;
	}
	bool actor_family::handle([[maybe_unused]] const message<mforward<pbnet::PROBUFF_NET_FAMIL_INFO>>& adata)
	{
		const auto* lparm = adata.get_data();
		i64_actorid lroleid = lparm->identifier();
		m_family.sync_family(lroleid);
		return true;
	}
	bool actor_family::handle([[maybe_unused]] const message<mforward<pbnet::PROBUFF_NET_FAMIL_JOIN>>& adata)
	{
		const auto* lparm = adata.get_data();
		const auto* lrecv = lparm->data();
		i64_actorid lroleid = lparm->identifier();
		i64_actorid familid = lrecv->mfamilid();

		pbnet::PROBUFF_NET_FAMIL_JOIN_RESPONSE pro;
		int32_t lstat = 0;
		if (lrecv->mapply())
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
	bool actor_family::handle([[maybe_unused]] const message<mforward<pbnet::PROBUFF_NET_FAMIL_LEAVE>>& adata)
	{
		const auto* lparm = adata.get_data();
		const auto* lrecv = lparm->data();
		i64_actorid lroleid = lparm->identifier();
		i64_actorid familid = lrecv->mfamilid();

		pbnet::PROBUFF_NET_FAMIL_LEAVE_RESPONSE pro;
		int32_t lstat = m_family.leave_family(lroleid, familid);
		pro.set_mstat(lstat);
		send_client(lroleid, pro);
		return true;
	}
	bool actor_family::handle([[maybe_unused]] const message<mforward<pbnet::PROBUFF_NET_FAMIL_LIST>>& adata)
	{
		const auto* lparm = adata.get_data();
		m_family.sync_familylist(lparm->identifier(), lparm->data()->mfamilid());
		return true;
	}
	bool actor_family::handle([[maybe_unused]] const message<mforward<pbnet::PROBUFF_NET_FAMIL_RATIFY_JOIN>>& adata)
	{
		const auto* lparm = adata.get_data();
		const auto* lrecv = lparm->data();
		i64_actorid lroleid = lparm->identifier();
		i64_actorid ljoinroleid = lrecv->mroleid();

		pbnet::PROBUFF_NET_FAMIL_RATIFY_JOIN_RESPONSE pro;
		int32_t lstat = m_family.ratify_join_family(lroleid, ljoinroleid, lrecv->mratify());
		pro.set_mstat(lstat);
		send_client(lroleid, pro);
		return true;
	}
	bool actor_family::handle([[maybe_unused]] const message<mforward<pbnet::PROBUFF_NET_FAMIL_SIGN>>& adata)
	{
		const auto* lparm = adata.get_data();
		const auto* lrecv = lparm->data();
		i64_actorid lroleid = lparm->identifier();
		i64_actorid familid = lrecv->mfamilid();

		pbnet::PROBUFF_NET_FAMIL_SIGN_RESPONSE pro;
		int32_t lstat = m_family.sign_family(lroleid, familid);
		pro.set_mstat(lstat);
		send_client(lroleid, pro);
		return true;
	}
	bool actor_family::handle([[maybe_unused]] const message<np_eevents_logic_rolelogin>& adata)
	{
		const auto* lrecv = adata.get_data();
		std::vector<i64_actorid> lfamilyers;
		if (m_family.get_familyers(lrecv->m_actorid, lfamilyers))
		{
			pbnet::PROBUFF_NET_ROLESTAT pro;
			pro.set_mstat(pbnet::PROBUFF_NET_ROLESTAT::online);
			pro.set_mlogicstat(pbnet::PROBUFF_NET_ROLESTAT::familyer);
			pro.set_mroleid(lrecv->m_actorid);
			send_client(lfamilyers, pro);
		}
		return true;
	}
	bool actor_family::handle([[maybe_unused]] const message<np_eevents_logic_roleoffline>& adata)
	{
		const auto* lrecv = adata.get_data();
		std::vector<i64_actorid> lfamilyers;
		if (m_family.get_familyers(lrecv->m_actorid, lfamilyers))
		{
			pbnet::PROBUFF_NET_ROLESTAT pro;
			pro.set_mstat(pbnet::PROBUFF_NET_ROLESTAT::offline);
			pro.set_mlogicstat(pbnet::PROBUFF_NET_ROLESTAT::familyer);
			pro.set_mroleid(lrecv->m_actorid);
			send_client(lfamilyers, pro);
		}
		return true;
	}
}//namespace ngl