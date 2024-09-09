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
			return nguid::make(ACTOR_MAIL, area, nguid::none_actordataid());
		}

		// # 发送邮件,根据掉落id，允许任意位置调用
		static bool sendmail(
			i64_actorid aactorid, int32_t amailid, int32_t adropid, const std::string& aparm
		);

		static bool sendmail(
			i64_actorid aactorid, int32_t amailid, const std::map<int32_t, int32_t>& aitems, const std::string& aparm
		);

		// # ACTOR_PROTOCOL_ADD_MAIL,新增邮件
		bool handle(const message<np_actor_addmail>& adata);

		using handle_cmd = cmd<actor_mail, std::string, int, const ngl::json_read&>;

		// # GM操作mail
		bool handle(const message<mforward<np_gm>>& adata);

		// # 获取邮件列表
		bool handle(const message<mforward<pbnet::PROBUFF_NET_MAIL_LIST>>& adata);
		
		// # 读取邮件
		bool handle(const message<mforward<pbnet::PROBUFF_NET_MAIL_READ>>& adata);

		// # 领取邮件附件
		bool handle(const message<mforward<pbnet::PROBUFF_NET_MAIL_DRAW>>& adata);
		
		// # 删除邮件
		bool handle(const message<mforward<pbnet::PROBUFF_NET_MAIL_DEL>>& adata);
	};
}// namespace ngl