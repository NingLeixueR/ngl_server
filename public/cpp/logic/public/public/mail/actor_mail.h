#pragma once

#include "actor_manage.h"
#include "actor_create.h"
#include "ndb_modular.h"
#include "nroleitems.h"
#include "ndbclient.h"
#include "nprotocol.h"
#include "ntimer.h"
#include "nlog.h"
#include "mail.h"
#include "net.h"
#include "db_manage.h"
#include "db_pool.h"
#include "db_data.h"
#include "db.h"

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
		static void nregister();

		virtual ~actor_mail() {}

		virtual void loaddb_finish(bool adbishave) {}

		static bool sendmail(i64_actorid actorid, int32_t amailid, int32_t adropid, const std::string& aparm)
		{
			auto pro = std::make_shared<np_actor_addmail>();
			pro->m_roleid = actorid;
			pro->m_tid = amailid;
			if (drop::droplist(adropid, 1, pro->m_items) == false)
			{
				LogLocalError("drop[%] not find!!!", adropid);
				return false;
			}
			pro->m_parm = aparm;
			actor::static_send_actor(
				nguid::make(ACTOR_MAIL, nguid::area(actorid), nguid::none_actordataid())
				, nguid::make()
				, pro
			);
			return true;
		}

		// ---- ACTOR_PROTOCOL_ADD_MAIL,ÐÂÔöÓÊ¼þ
		bool handle(message<np_actor_addmail>& adata)
		{
			auto lparm = adata.m_data;
			m_mails.addmail(lparm->m_roleid, lparm->m_tid, lparm->m_items, lparm->m_parm);
			return true;
		}
		
		bool handle(message<mforward<pbnet::PROBUFF_NET_MAIL_READ>>& adata)
		{
			auto lparm = adata.m_data;
			pbnet::PROBUFF_NET_MAIL_READ* lpdata = lparm->data();
			if (lpdata == nullptr)
				return true;
			pbnet::PROBUFF_NET_MAIL_READ_RESPONSE pro;
			pro.set_m_mailid(lpdata->m_mailid());
			pro.set_m_stat(m_mails.readmail(lparm->identifier(), lpdata->m_mailid()));
			return true;
		}

		bool handle(message<mforward<pbnet::PROBUFF_NET_MAIL_DRAW>>& adata)
		{
			auto lparm = adata.m_data;
			pbnet::PROBUFF_NET_MAIL_DRAW* lpdata = lparm->data();
			if (lpdata == nullptr)
				return true;
			pbnet::PROBUFF_NET_MAIL_DRAW_RESPONSE pro;
			pro.set_m_mailid(lpdata->m_mailid());
			pro.set_m_stat(m_mails.drawmail(lparm->identifier(), lpdata->m_mailid()));
			return true;
		}
		
		bool handle(message<mforward<pbnet::PROBUFF_NET_MAIL_DEL>>& adata)
		{
			auto lparm = adata.m_data;
			pbnet::PROBUFF_NET_MAIL_DEL* lpdata = lparm->data();
			if (lpdata == nullptr)
				return true;
			pbnet::PROBUFF_NET_MAIL_DEL_RESPONSE pro;
			pro.set_m_mailid(lpdata->m_mailid());
			pro.set_m_stat(m_mails.delmail(lparm->identifier(), lpdata->m_mailid()));
			return true;
		}
	};
}