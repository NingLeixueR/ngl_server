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
// File overview: Implements logic for redis.


#include "actor/protocol/nprotocol.h"
#include "tools/db/redis/dbredis.h"
#include "actor/actor_base/core/nguid.h"
#include "tools/tab/xml/xml.h"

namespace ngl
{
	redisReply* redis_cmd::cmd(redisContext* arc, const char* format, ...)
	{
		redisReply* lreply;
		if (arc == nullptr)
		{
			return nullptr;
		}
		va_list lvalist;
		va_start(lvalist, format);
		lreply = (redisReply*)redisvCommand(arc, format, lvalist);
		va_end(lvalist);
		if (lreply == nullptr || lreply->type == REDIS_REPLY_ERROR)
		{
			// A broken reply invalidates the current hiredis context; callers are
			// expected to recreate the wrapper instead of reusing it.
			if (lreply)
			{
				log_error()->print("[ERROR] Redis[{}] ", lreply->str);
				freeReplyObject(lreply);
			}
			redisFree(arc);
			return nullptr;
		}
		return lreply;
	}

	bool redis_cmd::del(redisContext* arc, const char* atab, int aid)
	{
		redisReply* lreply = cmd(arc, "DEL %s:%d", atab, aid);
		if (lreply != nullptr)
		{
			freeReplyObject(lreply);
			return true;
		}
		return false;
	}

	redis::redis(const xarg_redis& aarg) :
		m_rc(nullptr)
	{
		m_rc = redisConnect(aarg.m_ip.c_str(), aarg.m_port);
		if (m_rc->err)
		{
			log_error()->print("[ERROR] Redis[{}] ", m_rc->err);
			tools::no_core_dump();
			return;
		}
		if (!redis_cmd::cmd(m_rc, "AUTH %s", aarg.m_passworld.c_str()))
		{
			tools::no_core_dump();
			return;
		}
	}
}

#include "actor/tab/ttab_servers.h"

namespace ngl
{
	void test_hiredis()
	{
		ngl::redis lredis(nconfig.redis());

		for (int i = 1; i < 10; ++i)
		{
			pbdb::db_role ltemp;
			
			ngl::i64_actorid lid = ngl::nguid::make(ngl::ACTOR_ROLE, tab_self_area, i);
			ltemp.set_mid(lid);
			pbdb::db_brief lrolebase;
			lrolebase.set_mid(lid);
			lrolebase.mutable_mbase()->set_mname(std::string("libo") + tools::lexical_cast<std::string>(i));
			lrolebase.mutable_mbase()->set_mlv(i);
			lrolebase.mutable_mbase()->set_mmoneygold(i + 1000);
			lrolebase.mutable_mbase()->set_mmoneysilver(i + 2000);
			lredis.set(i, ltemp);
		}

		for (int i = 1; i < 10; ++i)
		{
			pbdb::db_role ltemp;
			lredis.get(i, ltemp);
		}
		

		np_connect_actor_server ltempk;

		ltempk.m_serverid = 1989;
		ltempk.m_session = 519;
		lredis.set(322, ltempk);

		np_connect_actor_server ltempk2;
		lredis.get(322, ltempk2);

		std::cout << std::endl;
	}
}// namespace ngl
