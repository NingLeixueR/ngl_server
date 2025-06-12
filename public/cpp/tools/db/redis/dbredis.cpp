#include "nprotocol.h"
#include "dbredis.h"
#include "nguid.h"

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

	redis::redis(const redis_arg& arg) :
		m_arg(arg),
		m_rc(nullptr)
	{
		m_rc = redisConnect(arg.m_ip.c_str(), arg.m_port);
		if (m_rc->err)
		{
			log_error()->print("[ERROR] Redis[{}] ", m_rc->err);
			tools::core_dump();
			return;
		}
		if (!redis_cmd::cmd(m_rc, "AUTH %s", arg.m_passworld.c_str()))
		{
			tools::core_dump();
			return;
		}
	}
}

#include "ttab_servers.h"

namespace ngl
{
	void test_hiredis()
	{
		ngl::redis_arg larg;
		larg.m_ip = "127.0.0.1";
		larg.m_port = 6379;
		larg.m_passworld = "123456";
		ngl::redis lredis(larg);

		for (int i = 1; i < 10; ++i)
		{
			protobuf_data<pbdb::db_role> ltemp;
			ltemp.make();
			
			ngl::i64_actorid lid = ngl::nguid::make(ngl::ACTOR_ROLE, tab_self_area, i);
			ltemp->set_m_id(lid);
			pbdb::db_brief lrolebase;
			lrolebase.set_m_id(lid);
			lrolebase.set_m_name(std::string("libo") + ngl::tools::lexical_cast<std::string>(i));
			lrolebase.set_m_lv(i);
			lrolebase.set_m_moneygold(i + 1000);
			lrolebase.set_m_moneysilver(i + 2000);
			lredis.set(i, ltemp);
		}

		for (int i = 1; i < 10; ++i)
		{
			protobuf_data<pbdb::db_role> ltemp2;
			lredis.get(i, ltemp2);
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