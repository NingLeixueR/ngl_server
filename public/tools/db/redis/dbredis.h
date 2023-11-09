#pragma once

#include "hiredis.h"
#include "nlog.h"
#include "serialize.h"
#include "type.h"

#define REDIS_DATA_MAX (10240)

namespace ngl
{
	struct redis_arg
	{
		str_ip m_ip;
		i16_port m_port;
		std::string m_passworld;
	};

	class redis_cmd
	{
		redisContext* m_rc;
		redis_cmd();
	public:
		redis_cmd(redisContext* arc);

		redisReply* cmd(const char* format, ...);

		template <typename T>
		bool get(const char* atab, int akey, T& adata)
		{
			bool lret = false;
			redisReply* lreply = cmd("GET %s:%d", atab, akey);
			if (lreply != nullptr)
			{
				ngl::unserialize lunserialize((const char*)lreply->str, lreply->len);
				lret = adata.pop(lunserialize);
				freeReplyObject(lreply);
			}
			return lret;
		}

		template <typename T>
		bool get(const char* atab, std::map<int, T>& adata)
		{
			bool lret = false;
			redisReply* lreply = cmd("KEYS %s:*", atab);
			if (lreply != nullptr)
			{
				for (int i = 0; i < lreply->elements; ++i)
				{
					if (lreply->element[i]->type != REDIS_REPLY_STRING)
					{
						continue;
					}
					//tab:key
					std::size_t found = std::string(lreply->element[i]->str).find(':');
					int lid = 0;
					if (found != std::string::npos)
					{
						lid = boost::lexical_cast<int>(&(lreply->element[i]->str[found + 1]));
						get(atab, lid, adata[lid]);
					}
				}
				lret = true;
				freeReplyObject(lreply);
			}
			return lret;
		}

		template <typename T>
		bool set(const char* atab, int akey, T& adata)
		{
			bool lret = false;
			char lbuff[REDIS_DATA_MAX] = { 0x0 };
			ngl::serialize lflow(lbuff, REDIS_DATA_MAX);
			if (adata.push(lflow))
			{
				redisReply* lreply = cmd("SET %s:%d %b", atab, akey, lflow.buff(), (size_t)lflow.byte());
				if (lreply != nullptr)
				{
					freeReplyObject(lreply);
				}
			}
			return lret;
		}

		bool del(const char* atab, int aid);
	};

	class redis
	{
		redisContext* m_rc;
		redis_arg m_arg;
		redis();
	public:
		redis(const redis_arg& arg);

		template <typename T>
		bool get(int akey, T& adata)
		{
			redis_cmd lcmd(m_rc);
			return lcmd.get(T::name(), akey, adata);
		}

		template <typename T>
		bool get(std::map<int, T>& adata)
		{
			redis_cmd lcmd(m_rc);
			return lcmd.get(T::name(), adata);
		}

		template <typename T>
		bool set(int akey, T& adata)
		{
			redis_cmd lcmd(m_rc);
			return lcmd.set(T::name(), akey, adata);
		}

		template <typename T>
		bool del(int aid)
		{
			redis_cmd lcmd(m_rc);
			return lcmd.del(T::name(), aid);
		}
	};

	void test_hiredis();
}//namespace ngl