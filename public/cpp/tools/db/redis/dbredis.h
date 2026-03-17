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
// File overview: Declares interfaces for redis.

#pragma once

#include "tools/serialize/nserialize.h"
#include "tools/db/sql/db_buff.h"
#include "hiredis/hiredis.h"
#include "tools/log/nlog.h"
#include "tools/type.h"

#define REDIS_DATA_MAX (10240)

namespace ngl
{
	struct redis_cmd
	{
		// Small hiredis wrapper that standardizes error handling and key naming.
		static redisReply* cmd(redisContext* arc, const char* format, ...);

		template <typename T>
		static bool get(redisContext* arc, const char* atab, int akey, T& adata)
		{
			redisReply* lreply = cmd(arc, "GET %s:%d", atab, akey);
			if (lreply != nullptr)
			{
				// Values are stored as raw custom-serializer blobs.
				ngl::ser::serialize_pop lserialize((const char*)lreply->str, (int32_t)lreply->len);
				bool lsuccess =  ngl::ser::nserialize::pop(&lserialize, adata);
				freeReplyObject(lreply);
				return lsuccess;
			}
			return false;
		}

		template <typename T>
		static bool get(redisContext* arc, const char* atab, std::map<int, T>& adata)
		{
			redisReply* lreply = cmd(arc, "KEYS %s:*", atab);
			if (lreply != nullptr)
			{
				// This is a simple admin-style bulk read helper, not a streaming
				// production scan.
				for (int i = 0; i < lreply->elements; ++i)
				{
					if (lreply->element[i]->type != REDIS_REPLY_STRING)
					{
						continue;
					}
					//tab:key
					std::size_t found = std::string(lreply->element[i]->str).find(':');
					if (found != std::string::npos)
					{
						int lid = tools::lexical_cast<int>(&(lreply->element[i]->str[found + 1]));
						get(atab, lid, adata[lid]);
					}
				}
				freeReplyObject(lreply);
				return true;
			}
			return false;
		}

		template <typename T>
		static bool set(redisContext* arc, const char* atab, int akey, const T& adata)
		{
			char lbuff[REDIS_DATA_MAX] = { 0x0 };

			ngl::ser::serialize_push lserialize(lbuff, REDIS_DATA_MAX);
			bool lsuccess = ngl::ser::nserialize::push(&lserialize, adata);
			if (lsuccess)
			{
				redisReply* lreply = cmd(arc, "SET %s:%d %b", atab, akey, lserialize.buff(), (size_t)lserialize.pos());
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
		redisContext*		m_rc = nullptr;
	public:
		// Opens one authenticated redis connection using the XML config.
		redis(const xarg_redis& aarg);

		template <typename T>
		bool get(int akey, T& adata)
		{
			return redis_cmd::set(m_rc, tools::type_name<T>().c_str(), akey, adata);
		}

		template <typename T>
		bool get(std::map<int, T>& adata)
		{
			return redis_cmd::get(m_rc, tools::type_name<T>(), adata);
		}

		template <typename T>
		bool set(int akey, const T& adata)
		{
			return redis_cmd::set(m_rc, tools::type_name<T>().c_str(), akey, adata);
		}

		template <typename T>
		bool set(const std::map<int, T>& adata)
		{
			for (auto& [key, value] : adata)
			{
				if (!set(key, value))
				{
					return false;
				}
			}
			return true;
		}

		template <typename T>
		bool del(int aid)
		{
			return redis_cmd::del(m_rc, tools::type_name<T>(), aid);
		}
	};

	void test_hiredis();
}// namespace ngl
