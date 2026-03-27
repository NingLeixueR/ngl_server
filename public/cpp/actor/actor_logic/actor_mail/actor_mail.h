/*
* Copyright (c) [2020-2025] NingLeixueR
* 
* Project name: ngl_server
* Project URL: https://github.com/NingLeixueR/ngl_server
* 
* This file is part of the ngl_server project and is distributed under the MIT License.
* You may use, modify, and distribute this project under the license, including commercial use,
* but you must retain the original copyright and license notice.
* 
* For license details, see the LICENSE file in the project root:
* https://github.com/NingLeixueR/ngl_server/blob/main/LICENSE
*/
// File overview: Declares interfaces for actor mail.

#pragma once

#include "actor/actor_logic/actor_role/logic/bag/drop.h"
#include "actor/actor_logic/actor_mail/mail.h"
#include "tools/db/sql/mysql/nmysql_manage.h"
#include "tools/db/sql/mysql/nmysql_pool.h"
#include "actor/actor_base/core/actor_manage.h"
#include "actor/actor_logic/actor_create/actor_create.h"
#include "actor/actor_base/core/ndb_modular.h"
#include "actor/actor_base/core/ndbclient.h"
#include "tools/db/sql/mysql/nmysql.h"
#include "actor/protocol/nprotocol.h"
#include "actor/actor_base/core/ntimer.h"
#include "tools/db/sql/db_data.h"
#include "tools/log/nlog.h"
#include "net/tcp/ntcp.h"

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

		// # Sendmail, id, position
		static bool sendmail(i64_actorid aactorid, int32_t amailid, int32_t adropid, const std::string& aparm);

		using handle_cmd = tools::cmd<actor_mail, std::string, int, ncjson&>;

		bool timer_handle([[maybe_unused]] const message<np_timerparm>& adata);

		bool handle([[maybe_unused]] const message<mforward<np_gm>>& adata);
		bool handle([[maybe_unused]] const message<mforward<pbnet::PROBUFF_NET_MAIL_DEL>>& adata);
		bool handle([[maybe_unused]] const message<mforward<pbnet::PROBUFF_NET_MAIL_DRAW>>& adata);
		bool handle([[maybe_unused]] const message<mforward<pbnet::PROBUFF_NET_MAIL_LIST>>& adata);
		bool handle([[maybe_unused]] const message<mforward<pbnet::PROBUFF_NET_MAIL_READ>>& adata);
		bool handle([[maybe_unused]] const message<np_actor_addmail>& adata);
	};
}// namespace ngl
