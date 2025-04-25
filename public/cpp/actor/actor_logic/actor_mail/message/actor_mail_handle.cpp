#include "actor_mail.h"
namespace ngl
{
	bool actor_mail::handle(const message<mforward<np_gm>>& adata)
	{
		ngl::json_read lojson(adata.get_data()->data()->m_json.c_str());
		std::string loperator;
		if (lojson.read("operator", loperator) == false)
		{
			return true;
		}
		if (handle_cmd::empty())
		{
			handle_cmd::push("get_mails", [this](int id, const ngl::json_read& aos)
				{
					gcmd<std::string> pro(id, "get_mails");
					int64_t roleid = 0;
					if (aos.read("data", roleid) == false)
					{
						return;
					}
					const pbdb::db_mail* ldb = m_mails.get_db_mail(roleid);
					if (ldb == nullptr)
					{
						return;
					}
					tools::proto2json(*ldb, pro.m_data);
					pro.m_istoutf8 = false;
				}
			);
			handle_cmd::push("add_mail", [this](int id, const ngl::json_read& aos)
				{
					gcmd<bool> pro(id, "add_mail", false);
					struct gm_mailitem
					{
						int32_t m_itemtid = -1;
						int32_t m_count;
						jsonfunc("itemtid", m_itemtid, "itemtcount", m_count)
					};
					struct gm_mail
					{
						int64_t m_roleid = nguid::make();
						std::string m_content;				// 邮件内容
						std::vector<gm_mailitem>  m_items;	// 邮件附件
						jsonfunc("roleid", m_roleid, "content", m_content, "items", m_items)
					};
					gm_mail recv;
					if (aos.read("data", recv) == false)
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
				}
			);
			handle_cmd::push("del_mail", [this](int id, const ngl::json_read& aos)
				{
					gcmd<bool> pro(id, "del_mail", false);
					struct gm_deletemail
					{
						int64_t m_roleid = nguid::make();
						int32_t m_mailid = -1;
						jsonfunc("roleid", m_roleid, "mailid", m_mailid)
					};
					gm_deletemail ldelmail;
					if (aos.read("data", ldelmail) == false)
					{
						return;
					}
					m_mails.delmail(ldelmail.m_roleid, ldelmail.m_mailid, false);
					pro.m_data = true;
				}
			);
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
		const pbnet::PROBUFF_NET_MAIL_DEL* lpdata = lparm->data();
		if (lpdata == nullptr)
		{
			return true;
		}
		pbnet::PROBUFF_NET_MAIL_DEL_RESPONSE pro;
		pro.set_m_mailid(lpdata->m_mailid());
		pro.set_m_stat(m_mails.delmail(lparm->identifier(), lpdata->m_mailid()));
		return true;
	}
	bool actor_mail::handle(const message<mforward<pbnet::PROBUFF_NET_MAIL_DRAW>>& adata)
	{
		auto lparm = adata.get_data();
		const pbnet::PROBUFF_NET_MAIL_DRAW* lpdata = lparm->data();
		if (lpdata == nullptr)
		{
			return true;
		}
		pbnet::PROBUFF_NET_MAIL_DRAW_RESPONSE pro;
		pro.set_m_mailid(lpdata->m_mailid());
		pro.set_m_stat(m_mails.drawmail(lparm->identifier(), lpdata->m_mailid()));
		return true;
	}
	bool actor_mail::handle(const message<mforward<pbnet::PROBUFF_NET_MAIL_LIST>>& adata)
	{
		auto lparm = adata.get_data();
		const pbnet::PROBUFF_NET_MAIL_LIST* lpdata = lparm->data();
		if (lpdata == nullptr)
		{
			return true;
		}
		i64_actorid roleid = lparm->identifier();
		auto pro = m_mails.sync_mail(roleid);
		if (pro == nullptr)
		{
			return true;
		}
		send_client(roleid, pro);
		return true;
	}
	bool actor_mail::handle(const message<mforward<pbnet::PROBUFF_NET_MAIL_READ>>& adata)
	{
		auto lparm = adata.get_data();
		const pbnet::PROBUFF_NET_MAIL_READ* lpdata = lparm->data();
		if (lpdata == nullptr)
		{
			return true;
		}
		pbnet::PROBUFF_NET_MAIL_READ_RESPONSE pro;
		pro.set_m_mailid(lpdata->m_mailid());
		pro.set_m_stat(m_mails.readmail(lparm->identifier(), lpdata->m_mailid()));
		return true;
	}
	bool actor_mail::handle(const message<np_actor_addmail>& adata)
	{
		auto lparm = adata.get_data();
		m_mails.addmail(lparm->m_roleid, lparm->m_tid, lparm->m_items, lparm->m_parm);
		return true;
	}
}//namespace ngl
