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
#include "drop.h"
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

		static i64_actorid actorid(i16_area area)
		{
			return nguid::make(ACTOR_MAIL, area, nguid::none_actordataid());
		}

		static bool sendmail(i64_actorid aactorid, int32_t amailid, int32_t adropid, const std::string& aparm)
		{
			auto pro = std::make_shared<np_actor_addmail>();
			pro->m_roleid = aactorid;
			pro->m_tid = amailid;
			if (drop::droplist(adropid, 1, pro->m_items) == false)
			{
				ngl::log_error()->print("drop[{}] not find!!!", adropid);
				return false;
			}
			pro->m_parm = aparm;
			actor::static_send_actor(actorid(nguid::area(aactorid)) , nguid::make() , pro);
			return true;
		}

		// ---- ACTOR_PROTOCOL_ADD_MAIL,新增邮件
		bool handle(message<np_actor_addmail>& adata)
		{
			auto lparm = adata.m_data;
			m_mails.addmail(lparm->m_roleid, lparm->m_tid, lparm->m_items, lparm->m_parm);
			return true;
		}

		using handle_cmd = cmd<actor_mail, std::string, int, ngl::ojson&>;
		// ## GM操作mail
		bool handle(message<mforward<np_gm>>& adata);

		bool handle(message<mforward<pbnet::PROBUFF_NET_MAIL_LIST>>& adata)
		{
			auto lparm = adata.m_data;
			pbnet::PROBUFF_NET_MAIL_LIST* lpdata = lparm->data();
			if (lpdata == nullptr)
				return true;
			i64_actorid roleid = adata.m_data->identifier();
			auto pro = m_mails.sync_mail(roleid);
			if (pro == nullptr)
				return true;
			send_client(roleid, pro);
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