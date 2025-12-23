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
#include "actor_mail.h"
namespace ngl
{
	bool actor_mail::handle(const message<mforward<np_gm>>& adata)
	{
		ngl::njson_read lojson(adata.get_data()->data()->m_json.c_str());
		std::string loperator;
		if (!njson::read(lojson, "operator", loperator))
		{
			return true;
		}
		if (handle_cmd::empty())
		{
			handle_cmd::add("get_mails") = [this](int id, ngl::njson_read& aos)
				{
					gcmd<std::string> pro(id, "get_mails");
					int64_t roleid = 0;
					if (!njson::read(aos, "data", roleid))
					{
						return;
					}
					data_modified<pbdb::db_mail>& lpdbmodifiedmail = m_mails.get(id);
					data_modified_return_getconst(lpdbmailconst, lpdbmodifiedmail);
					tools::proto2json(*lpdbmailconst, pro.m_data);
					pro.m_istoutf8 = false;
				};

			handle_cmd::add("add_mail") = [this](int id, ngl::njson_read& aos)
				{
					gcmd<bool> pro(id, "add_mail", false);
					struct gm_mailitem
					{
						int32_t m_itemtid = -1;
						int32_t m_count;
						dprotocol(gm_mailitem, m_itemtid, m_count)
					};
					struct gm_mail
					{
						int64_t m_roleid = nguid::make();
						std::string m_content;				// 邮件内容
						std::vector<gm_mailitem>  m_items;	// 邮件附件
						dprotocol(gm_mail, m_roleid, m_content, m_items)
					};
					gm_mail recv;
					if (!njson::read(aos, "data", recv))
					{
						return;
					}
					std::map<int32_t, int32_t> litem;
					for (gm_mailitem& gmailitem : recv.m_items)
					{
						litem[gmailitem.m_itemtid] += gmailitem.m_count;
					}
					if (m_mails.addmail(recv.m_roleid, litem, recv.m_content) == false)
					{
						return;
					}
					pro.m_data = true;
				};

			handle_cmd::add("del_mail") = [this](int id, ngl::njson_read& aos)
				{
					gcmd<bool> pro(id, "del_mail", false);
					struct gm_deletemail
					{
						int64_t m_roleid = nguid::make();
						int32_t m_mailid = -1;
						dprotocol(gm_deletemail, m_roleid, m_mailid)
					};
					gm_deletemail ldelmail;
					if (!njson::read(aos, "data", ldelmail))
					{
						return;
					}
					m_mails.delmail(ldelmail.m_roleid, ldelmail.m_mailid, false);
					pro.m_data = true;
				};
		}
		if (handle_cmd::function(loperator, (int)adata.get_data()->identifier(), lojson) == false)
		{
			log_error()->print("GM actor_mail operator[{}] ERROR", loperator);
		}
		return true;
	}
	bool actor_mail::handle(const message<mforward<pbnet::PROBUFF_NET_MAIL_DEL>>& adata)
	{
		auto lparm = adata.get_data();
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
		auto lparm = adata.get_data();
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
		auto lparm = adata.get_data();
		if (lparm == nullptr)
		{
			return true;
		}
		const pbnet::PROBUFF_NET_MAIL_LIST* lpdata = lparm->data();
		if (lpdata == nullptr)
		{
			return true;
		}
		i64_actorid roleid = lparm->identifier();
		pbnet::PROBUFF_NET_MAIL_LIST_RESPONSE pro;
		if (!m_mails.sync_mail(roleid, -1, pro))
		{
			return true;
		}
		send_client(roleid, pro);
		return true;
	}
	bool actor_mail::handle(const message<mforward<pbnet::PROBUFF_NET_MAIL_READ>>& adata)
	{
		auto lparm = adata.get_data();
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
		auto lparm = adata.get_data();

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
