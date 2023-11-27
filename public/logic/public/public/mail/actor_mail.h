#pragma once

#include "actor_manage.h"
#include "net.h"
#include "db_data.h"
#include "db.h"
#include "db_pool.h"
#include "db_manage.h"
#include "actor_db_client.h"
#include "actor_protocol.h"
#include "actor_timer.h"
#include "actor_create.h"

#include "db_modular.h"
#include "mail.h"
#include "actor_roleitem.h"

namespace ngl
{
	class actor_mail : public actor
	{
		mails m_mails;

		actor_mail();
	public:
		friend class actor_instance<actor_mail>;
		static actor_mail& getInstance()
		{
			return actor_instance<actor_mail>::instance();
		}

		virtual void init()
		{
			m_mails.set(this);
		}
		static void actor_register();

		virtual ~actor_mail() {}

		virtual void loaddb_finish(bool adbishave) {}

		enum { ACTOR_TYPE = ACTOR_MAIL};

		// ---- ACTOR_PROTOCOL_ADD_MAIL,ÐÂÔöÓÊ¼þ
		bool handle(i32_threadid athread, const std::shared_ptr<pack>& apack, actor_protocol_add_mail& adata)
		{
			m_mails.addmail(adata.m_roleid, adata.m_tid, adata.m_items, adata.m_parm);

			return true;
		}
		
		bool handle(i32_threadid athread, const std::shared_ptr<pack>& apack, mforward<pbnet::PROBUFF_NET_MAIL_READ>& adata)
		{
			pbnet::PROBUFF_NET_MAIL_READ* lpdata = adata.data();
			if (lpdata == nullptr)
				return true;
			pbnet::PROBUFF_NET_MAIL_READ_RESPONSE pro;
			pro.set_m_mailid(lpdata->m_mailid());
			pro.set_m_stat(m_mails.readmail(adata.identifier(), lpdata->m_mailid()));
			return true;
		}

		bool handle(i32_threadid athread, const std::shared_ptr<pack>& apack, mforward<pbnet::PROBUFF_NET_MAIL_DRAW>& adata)
		{
			pbnet::PROBUFF_NET_MAIL_DRAW* lpdata = adata.data();
			if (lpdata == nullptr)
				return true;
			pbnet::PROBUFF_NET_MAIL_DRAW_RESPONSE pro;
			pro.set_m_mailid(lpdata->m_mailid());
			pro.set_m_stat(m_mails.drawmail(adata.identifier(), lpdata->m_mailid()));
			return true;
		}

		bool handle(i32_threadid athread, const std::shared_ptr<pack>& apack, mforward<pbnet::PROBUFF_NET_MAIL_DEL>& adata)
		{
			pbnet::PROBUFF_NET_MAIL_DEL* lpdata = adata.data();
			if (lpdata == nullptr)
				return true;
			pbnet::PROBUFF_NET_MAIL_DEL_RESPONSE pro;
			pro.set_m_mailid(lpdata->m_mailid());
			pro.set_m_stat(m_mails.delmail(adata.identifier(), lpdata->m_mailid()));
			return true;
		}

	
	private:
	};
}