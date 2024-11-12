#pragma once

#include "actor_manage.h"
#include "actor_create.h"
#include "ndb_modular.h"
#include "nsp_server.h"
#include "nsp_client.h"
#include "ndbclient.h"
#include "nprotocol.h"
#include "db_manage.h"
#include "db_pool.h"
#include "db_data.h"
#include "ntimer.h"
#include "drop.h"
#include "nlog.h"
#include "mail.h"
#include "net.h"
#include "db.h"

namespace ngl
{
	class actor_mail : public actor
	{
		actor_mail(const actor_mail&) = delete;
		actor_mail& operator=(const actor_mail&) = delete;

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

		static ENUM_ACTOR actor_type()
		{
			return ACTOR_MAIL;
		}

		static i64_actorid actorid(i16_area area)
		{
			return nguid::make(actor_type(), area, nguid::none_actordataid());
		}

		// # �����ʼ�,���ݵ���id����������λ�õ���
		static bool sendmail(
			i64_actorid aactorid, int32_t amailid, int32_t adropid, const std::string& aparm
		);

		static bool sendmail(
			i64_actorid aactorid, int32_t amailid, const std::map<int32_t, int32_t>& aitems, const std::string& aparm
		);

		// # ACTOR_PROTOCOL_ADD_MAIL,�����ʼ�
		bool handle(const message<np_actor_addmail>& adata);

		using handle_cmd = cmd<actor_mail, std::string, int, const ngl::json_read&>;

		// # GM����mail
		bool handle(const message<mforward<np_gm>>& adata);

		// # ��ȡ�ʼ��б�
		bool handle(const message<mforward<pbnet::PROBUFF_NET_MAIL_LIST>>& adata);
		
		// # ��ȡ�ʼ�
		bool handle(const message<mforward<pbnet::PROBUFF_NET_MAIL_READ>>& adata);

		// # ��ȡ�ʼ�����
		bool handle(const message<mforward<pbnet::PROBUFF_NET_MAIL_DRAW>>& adata);
		
		// # ɾ���ʼ�
		bool handle(const message<mforward<pbnet::PROBUFF_NET_MAIL_DEL>>& adata);
	};
}// namespace ngl