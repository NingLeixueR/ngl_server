/*
* Copyright (c) [2020-2025] NingLeixueR
* 
* 项目名称：ngl_server
* 项目地址：https://github.com/NingLeixueR/ngl_server
* 
* 本文件是 ngl_server 项目的一部分，遵循 MIT 开源协议发布。
* 您可以按照协议规定自由使用、修改和分发本项目，包括商业用途，
* 但需保留原始版权和许可声明。
* 
* 许可详情参见项目根目录下的 LICENSE 文件：
* https://github.com/NingLeixueR/ngl_server/blob/main/LICENSE
*/
#pragma once

#include "actor_manage.h"
#include "actor_create.h"
#include "ndb_modular.h"
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

		virtual void loaddb_finish(pbdb::ENUM_DB atype, enum_dbstat astat);

		static void nregister();

		drop<actor_mail>& get_drop()
		{
			return m_drop;
		}

		// # 发送邮件,根据掉落id，允许任意位置调用
		static bool sendmail(i64_actorid aactorid, int32_t amailid, int32_t adropid, const std::string& aparm);

		using handle_cmd = cmd<actor_mail, std::string, int, ncjson&>;

		bool timer_handle(const message<np_timerparm>& adata);

		bool handle(const message<mforward<np_gm>>& adata);
		bool handle(const message<mforward<pbnet::PROBUFF_NET_MAIL_DEL>>& adata);
		bool handle(const message<mforward<pbnet::PROBUFF_NET_MAIL_DRAW>>& adata);
		bool handle(const message<mforward<pbnet::PROBUFF_NET_MAIL_LIST>>& adata);
		bool handle(const message<mforward<pbnet::PROBUFF_NET_MAIL_READ>>& adata);
		bool handle(const message<np_actor_addmail>& adata);
	};
}// namespace ngl
