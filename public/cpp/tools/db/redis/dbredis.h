#pragma once

#include "serialize.h"
#include "hiredis.h"
#include "db_buff.h"
#include "nlog.h"
#include "type.h"

#define REDIS_DATA_MAX (10240)

namespace ngl
{
	struct redis_arg
	{
		str_ip		m_ip;
		i16_port	m_port;
		std::string m_passworld;
	};

	class redis_cmd
	{
	public:
		static redisReply* cmd(redisContext* arc, const char* format, ...);

		template <typename T>
		static bool get(redisContext* arc, const char* atab, int akey, T& adata)
		{
			redisReply* lreply = cmd(arc, "GET %s:%d", atab, akey);
			if (lreply != nullptr)
			{
				ngl::unserialize lunserialize((const char*)lreply->str, lreply->len);
				if (lunserialize.pop(adata))
				{
					freeReplyObject(lreply);
					return true;
				}
				else
				{
					freeReplyObject(lreply);
					return false;
				}
			}
			return false;
		}

		template <typename T>
		static bool get(redisContext* arc, const char* atab, std::map<int, protobuf_data<T>>& adata)
		{
			redisReply* lreply = cmd(arc, "KEYS %s:*", atab);
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
						lid = tools::lexical_cast<int>(&(lreply->element[i]->str[found + 1]));
						get(atab, lid, adata[lid]);
					}
				}
				freeReplyObject(lreply);
				return true;
			}
			return false;
		}

		template <typename T>
		static bool set(redisContext* arc, const char* atab, int akey, protobuf_data<T>& adata)
		{
			char lbuff[REDIS_DATA_MAX] = { 0x0 };
			ngl::serialize lflow(lbuff, REDIS_DATA_MAX);
			if (lflow.push(adata))
			{
				redisReply* lreply = cmd(arc, "SET %s:%d %b", atab, akey, lflow.buff(), (size_t)lflow.byte());
				if (lreply != nullptr)
				{
					freeReplyObject(lreply);
					return true;
				}
			}
			return false;
		}

		static bool del(redisContext* arc, const char* atab, int aid);
	};

	class redis
	{
		redisContext*	m_rc;
		redis_arg		m_arg;
		redis();
	public:
		redis(const redis_arg& arg);

		template <typename T>
		bool get(int akey, protobuf_data<T>& adata)
		{
			std::string lname = tools::protobuf_tabname<T>::name();
			return redis_cmd::get(m_rc, lname.c_str(), akey, adata);
		}

		template <typename T>
		bool get(std::map<int, protobuf_data<T>>& adata)
		{
			return redis_cmd::get(m_rc, T::name(), adata);
		}

		template <typename T>
		bool set(int akey, protobuf_data<T>& adata)
		{
			std::string lname = tools::protobuf_tabname<T>::name();
			return redis_cmd::set(m_rc, lname.c_str(), akey, adata);
		}

		template <typename T>
		bool del(int aid)
		{
			return redis_cmd::del(m_rc, T::name(), aid);
		}
	};

	void test_hiredis();
}// namespace ngl