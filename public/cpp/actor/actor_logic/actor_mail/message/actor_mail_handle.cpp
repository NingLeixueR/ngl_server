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
// File overview: Implements message handlers for message.
#include "actor/actor_logic/actor_mail/actor_mail.h"
namespace ngl
{
	bool actor_mail::handle(const message<mforward<np_gm>>& adata)
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
			handle_cmd::add("get_mails") = [this](int id, ncjson& aos)
				{
					gcmd<std::string> pro(id, "get_mails");
					int64_t roleid = 0;
					if (!njson::pop(aos, { "data" }, roleid))
					{
						return;
					}
					data_modified<pbdb::db_mail>& lpdbmodifiedmail = m_mails.get(id);
					MODIFIED_RETURN_CONST(lpdbmailconst, lpdbmodifiedmail);
					tools::proto2json(*lpdbmailconst, pro.m_data);
					pro.m_istoutf8 = false;
				};

			handle_cmd::add("add_mail") = [this](int id, ncjson& aos)
				{
					gcmd<bool> pro(id, "add_mail", false);
					struct gm_mailitem
					{
						int32_t m_itemtid = -1;
						int32_t m_count;
						DPROTOCOL(gm_mailitem, m_itemtid, m_count)
					};
					struct gm_mail
					{
						int64_t m_roleid = nguid::make();
						std::string m_content;				// Mailcontent
						std::vector<gm_mailitem>  m_items;	// Mailattachment
						DPROTOCOL(gm_mail, m_roleid, m_content, m_items)
					};
					gm_mail recv;
					if (!njson::pop(aos, { "data" }, recv))
					{
						return;
					}
					std::map<int32_t, int32_t> litem;
					for (const gm_mailitem& lmailitem : recv.m_items)
					{
						litem[lmailitem.m_itemtid] += lmailitem.m_count;
					}
					if (m_mails.addmail(recv.m_roleid, litem, recv.m_content) == false)
					{
						return;
					}
					pro.m_data = true;
				};

			handle_cmd::add("del_mail") = [this](int id, ncjson& aos)
				{
					gcmd<bool> pro(id, "del_mail", false);
					struct gm_deletemail
					{
						int64_t m_roleid = nguid::make();
						int32_t m_mailid = -1;
						DPROTOCOL(gm_deletemail, m_roleid, m_mailid)
					};
					gm_deletemail ldelmail;
					if (!njson::pop(aos, { "data" }, ldelmail))
					{
						return;
					}
					m_mails.delmail(ldelmail.m_roleid, ldelmail.m_mailid, false);
					pro.m_data = true;
				};
		}
		if (handle_cmd::function(loperator, (int)lparm->identifier(), lojson) == false)
		{
			log_error()->print("GM actor_mail operator[{}] ERROR", loperator);
		}
		return true;
	}
	bool actor_mail::handle(const message<mforward<pbnet::PROBUFF_NET_MAIL_DEL>>& adata)
	{
		const auto* lparm = adata.get_data();
		if (lparm == nullptr)
		{
			return true;
		}
		const pbnet::PROBUFF_NET_MAIL_DEL* lpdata = lparm->data();
		if (lpdata == nullptr)
		{
			return true;
		}
		pbnet::PROBUFF_NET_MAIL_DEL_RESPONSE pro;
		pro.set_mmailid(lpdata->mmailid());
		pro.set_mstat(m_mails.delmail(lparm->identifier(), lpdata->mmailid()));
		return true;
	}
	bool actor_mail::handle(const message<mforward<pbnet::PROBUFF_NET_MAIL_DRAW>>& adata)
	{
		const auto* lparm = adata.get_data();
		if (lparm == nullptr)
		{
			return true;
		}
		const pbnet::PROBUFF_NET_MAIL_DRAW* lpdata = lparm->data();
		if (lpdata == nullptr)
		{
			return true;
		}
		pbnet::PROBUFF_NET_MAIL_DRAW_RESPONSE pro;
		pro.set_mmailid(lpdata->mmailid());
		pro.set_mstat(m_mails.drawmail(lparm->identifier(), lpdata->mmailid()));
		return true;
	}
	bool actor_mail::handle(const message<mforward<pbnet::PROBUFF_NET_MAIL_LIST>>& adata)
	{
		const auto* lparm = adata.get_data();
		if (lparm == nullptr)
		{
			return true;
		}
		const pbnet::PROBUFF_NET_MAIL_LIST* lpdata = lparm->data();
		if (lpdata == nullptr)
		{
			return true;
		}
		i64_actorid lroleid = lparm->identifier();
		pbnet::PROBUFF_NET_MAIL_LIST_RESPONSE pro;
		if (!m_mails.sync_mail(lroleid, -1, pro))
		{
			return true;
		}
		send_client(lroleid, pro);
		return true;
	}
	bool actor_mail::handle(const message<mforward<pbnet::PROBUFF_NET_MAIL_READ>>& adata)
	{
		const auto* lparm = adata.get_data();
		if (lparm == nullptr)
		{
			return true;
		}
		const pbnet::PROBUFF_NET_MAIL_READ* lpdata = lparm->data();
		if (lpdata == nullptr)
		{
			return true;
		}
		pbnet::PROBUFF_NET_MAIL_READ_RESPONSE pro;
		pro.set_mmailid(lpdata->mmailid());
		pro.set_mstat(m_mails.readmail(lparm->identifier(), lpdata->mmailid()));
		return true;
	}
	bool actor_mail::handle(const message<np_actor_addmail>& adata)
	{
		const auto* lparm = adata.get_data();

		std::map<int32_t, int32_t> litems;
		if (instance().get_drop().droplist(lparm->m_dropid, 1, litems) == false)
		{
			ngl::log_error()->print("role:{} mailid:{} drop:{} parm:{} fail!!!", nguid(lparm->m_roleid), lparm->m_tid, lparm->m_dropid, lparm->m_parm);
			return false;
		}

		m_mails.addmail(lparm->m_roleid, lparm->m_tid, litems, lparm->m_parm);
		return true;
	}
}//namespace ngl
