#include "actor_mail.h"

namespace ngl
{
	bool actor_mail::handle(const message<np_actor_addmail>& adata)
	{
		auto lparm = adata.get_data();
		m_mails.addmail(lparm->m_roleid, lparm->m_tid, lparm->m_items, lparm->m_parm);
		return true;
	}

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
					serialize::proto_json(*ldb, pro.m_data);
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
						std::string m_content;				// �ʼ�����
						std::vector<gm_mailitem>  m_items;	// �ʼ�����
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
}//namespace ngl