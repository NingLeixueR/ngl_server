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
	class actor_mail : 
		public actor
	{
		actor_mail(const actor_mail&) = delete;
		actor_mail& operator=(const actor_mail&) = delete;

		mails m_mails;
		drop<actor_mail> m_drop;

		actor_mail();
	public:
		friend class actor_instance<actor_mail>;
		static actor_mail& instance()
		{
			return actor_instance<actor_mail>::instance();
		}
		virtual ~actor_mail() = default;

		static ENUM_ACTOR actor_type();

		static i64_actorid actorid(i16_area area);

		virtual void init();

		virtual void erase_actor_before();

		virtual void loaddb_finish(bool adbishave);

		static void nregister();

		drop<actor_mail>& get_drop()
		{
			return m_drop;
		}

		// # 发送邮件,根据掉落id，允许任意位置调用
		static bool sendmail(i64_actorid aactorid, int32_t amailid, int32_t adropid, const std::string& aparm);

		using handle_cmd = cmd<actor_mail, std::string, int, const ngl::json_read&>;

		bool timer_handle(const message<np_timerparm>& adata);

		bool handle(const message<np_arg_null>&);
		bool handle(const message<mforward<np_gm>>& adata);
		// [请求]领取邮件附件
		bool handle(const message<mforward<pbnet::PROBUFF_NET_MAIL_DEL>>& adata);
		// [请求]领取邮件附件
		bool handle(const message<mforward<pbnet::PROBUFF_NET_MAIL_DRAW>>& adata);
		// [请求]获取邮件列表
		bool handle(const message<mforward<pbnet::PROBUFF_NET_MAIL_LIST>>& adata);
		// [请求]读取邮件
		bool handle(const message<mforward<pbnet::PROBUFF_NET_MAIL_READ>>& adata);
		bool handle(const message<np_actor_addmail>& adata);
	};
}// namespace ngl
