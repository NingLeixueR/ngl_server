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
			handle_cmd::add("get_mails") = [this](int id, const ngl::json_read& aos)
				{
					gcmd<std::string> pro(id, "get_mails");
					int64_t roleid = 0;
					if (aos.read("data", roleid) == false)
					{
						return;
					}
					data_modified<pbdb::db_mail>& lpdb_mail = m_mails.get(id);
					tools::proto2json(lpdb_mail.get(), pro.m_data);
					pro.m_istoutf8 = false;
				};

			handle_cmd::add("add_mail") = [this](int id, const ngl::json_read& aos)
				{
					gcmd<bool> pro(id, "add_mail", false);
					struct gm_mailitem
					{
						int32_t m_itemtid = -1;
						int32_t m_count;
						def_jsonfunction(m_itemtid, m_count)
					};
					struct gm_mail
					{
						int64_t m_roleid = nguid::make();
						std::string m_content;				// �ʼ�����
						std::vector<gm_mailitem>  m_items;	// �ʼ�����
						def_jsonfunction(m_roleid, m_content, m_items)
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
				};

			handle_cmd::add("del_mail") = [this](int id, const ngl::json_read& aos)
				{
					gcmd<bool> pro(id, "del_mail", false);
					struct gm_deletemail
					{
						int64_t m_roleid = nguid::make();
						int32_t m_mailid = -1;
						def_jsonfunction(m_roleid, m_mailid)
					};
					gm_deletemail ldelmail;
					if (aos.read("data", ldelmail) == false)
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
			ngl::log_error()->print(
				"role:{} mailid:{} drop:{} parm:{} fail!!!"
				, nguid(lparm->m_roleid)
				, lparm->m_tid
				, lparm->m_dropid
				, lparm->m_parm
			);
			return false;
		}

		m_mails.addmail(lparm->m_roleid, lparm->m_tid, litems, lparm->m_parm);
		return true;
	}
}//namespace ngl
