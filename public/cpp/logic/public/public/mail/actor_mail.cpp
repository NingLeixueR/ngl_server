#include "actor_mail.h"

namespace ngl
{
	actor_mail::actor_mail() :
		actor(
			actorparm
			{
				.m_parm
				{
					.m_type = ACTOR_MAIL,
					.m_area = ttab_servers::tab()->m_area,
					.m_manage_dbclient = true
				},
				.m_weight = 0x7fffffff,
				.m_broadcast = true,
			})
	{
	}

	void actor_mail::nregister()
	{
		// 协议注册
		register_handle_custom<actor_mail>::func<
			np_actor_addmail
			, mforward<np_gm>
		>(true);

		register_handle_proto<actor_mail>::func<
			mforward<pbnet::PROBUFF_NET_MAIL_LIST>
			, mforward<pbnet::PROBUFF_NET_MAIL_READ>
			, mforward<pbnet::PROBUFF_NET_MAIL_DRAW>
			, mforward<pbnet::PROBUFF_NET_MAIL_DEL>
		>(true);
	}

	bool actor_mail::handle(message<mforward<np_gm>>& adata)
	{
		ngl::json_read lojson(adata.m_data->data()->m_json.c_str());
		std::string loperator;
		if (lojson.read("operator", loperator) == false)
		{
			return true;
		}
		if (handle_cmd::empty())
		{
			handle_cmd::push("get_mails", [this](int id, ngl::json_read& aos)
				{
					int64_t roleid = 0;
					if (aos.read("data", roleid) == false)
						return;
					gcmd<std::string> pro;
					pro.id = id;
					pro.m_operator = "get_mails_responce";
					const pbdb::db_mail* ldb = m_mails.get_db_mail(roleid);
					if (ldb == nullptr)
						return;
					serialize::proto_json(*ldb, pro.m_data);
					pro.m_istoutf8 = false;
				}
			);
			handle_cmd::push("add_mail", [this](int id, ngl::json_read& aos)
				{
					struct gm_mailitem
					{
						int32_t m_itemtid;
						int32_t m_count;
						jsonfunc("itemtid", m_itemtid, "itemtcount", m_count)
					};
					struct gm_mail
					{
						int64_t m_roleid;
						std::string m_content;				// 邮件内容
						std::vector<gm_mailitem>  m_items;	// 邮件附件

						jsonfunc("roleid", m_roleid, "content", m_content, "items", m_items)
					};
					// 返回 bool
					gm_mail recv;
					if (aos.read("data", recv) == false)
						return;
					std::map<int32_t, int32_t> litem;
					for (gm_mailitem& gmailitem : recv.m_items)
					{
						litem[gmailitem.m_itemtid] += gmailitem.m_count;
					}

					gcmd<bool> pro;
					pro.id = id;
					pro.m_operator = "add_mail_responce";
					pro.m_data = false;
					if (m_mails.addmail(recv.m_roleid, litem, recv.m_content) == false)
					{
						return;
					}
					pro.m_data = true;
				}
			);
			handle_cmd::push("del_mail", [this](int id, ngl::json_read& aos)
				{
					// 返回 bool
					struct gm_deletemail
					{
						int64_t m_roleid;
						int32_t m_mailid;
						jsonfunc("roleid", m_roleid, "mailid", m_mailid)
					};
					gm_deletemail ldelmail;
					if (aos.read("data", ldelmail) == false)
						return;
					gcmd<bool> pro;
					pro.id = id;
					pro.m_operator = "del_mail_responce";
					pro.m_data = true;
					m_mails.delmail(ldelmail.m_roleid, ldelmail.m_mailid, false);
				}
			);
		}

		if (handle_cmd::function(loperator, adata.m_data->identifier(), lojson) == false)
		{
			log_error()->print("GM actor_mail operator[{}] ERROR", loperator);
		}
		return true;
	}
}// namespace ngl