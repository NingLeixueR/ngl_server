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

#include "nserialize.h"
#include "hiredis/hiredis.h"
#include "db_buff.h"
#include "nlog.h"
#include "type.h"

#define REDIS_DATA_MAX (10240)

namespace ngl
{
	struct redis_cmd
	{
		static redisReply* cmd(redisContext* arc, const char* format, ...);

		template <typename T>
		static bool get(redisContext* arc, const char* atab, int akey, T& adata)
		{
			redisReply* lreply = cmd(arc, "GET %s:%d", atab, akey);
			if (lreply != nullptr)
			{
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