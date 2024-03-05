#include "dbredis.h"
#include "actor_guid.h"

namespace ngl
{
	redis_cmd::redis_cmd(redisContext* arc) :
		m_rc(arc) {}

	redisReply* redis_cmd::cmd(const char* format, ...)
	{
		redisReply* lreply;
		if (m_rc == nullptr)
		{
			return nullptr;
		}
		va_list lvalist;
		va_start(lvalist, format);
		lreply = (redisReply*)redisvCommand(m_rc, format, lvalist);
		va_end(lvalist);
		if (lreply == nullptr || lreply->type == REDIS_REPLY_ERROR)
		{
			if (lreply)
			{
				LogLocalError("[ERROR] Redis[%] ", lreply->str)
				freeReplyObject(lreply);
			}
			redisFree(m_rc);
			return nullptr;
		}
		return lreply;
	}

	bool redis_cmd::del(const char* atab, int aid)
	{
		bool lret = false;
		redisReply* lreply = cmd("DEL %s:%d", atab, aid);
		if (lreply != nullptr)
		{
			freeReplyObject(lreply);
			lreply = nullptr;
			lret = true;
		}
		return lret;
	}

	redis::redis(const redis_arg& arg) :
		m_arg(arg),
		m_rc(nullptr)
	{
		m_rc = redisConnect(arg.m_ip.c_str(), arg.m_port);
		if (m_rc->err)
		{
			LogLocalError("[ERROR] Redis[%] ", m_rc->err)
			assert(0);
			return;
		}
		redis_cmd lcmd(m_rc);
		if (!lcmd.cmd("AUTH %s", arg.m_passworld.c_str()))
		{
			assert(0);
			return;
		}
	}

	void test_hiredis()
	{
		//ngl::redis_arg larg;
		//larg.m_ip = "127.0.0.1";
		//larg.m_port = 6379;
		//larg.m_passworld = "123456";
		//ngl::redis lredis(larg);

		//for (int i = 1; i < 10; ++i)
		//{
		//	DB_ROLE ltemp;

		//	ltemp.mm_id() = ngl::actor_guid::make(ngl::ACTOR_ROLE, 1, i);
		//	ltemp.mm_name() = "libo";
		//	ltemp.mm_name() += boost::lexical_cast<std::string>(i);
		//	ltemp.mm_lv() = i;
		//	ltemp.mm_gold() = i;// 金币
		//	ltemp.mm_mapid() = i;// 所在地图
		//	// 当前位置 
		//	ltemp.mm_pos().x = i;
		//	ltemp.mm_pos().y = i;
		//	ltemp.mm_pos().z = i;
		//	// 方向向量(m_orientation * movespeed * 时间)
		//	ltemp.mm_orientation().x = i;
		//	ltemp.mm_orientation().y = i;
		//	ltemp.mm_orientation().z = i;
		//	lredis.set(i, ltemp);
		//}
		//DB_ROLE ltemp2;
		//lredis.get(1, ltemp2);
	}

}