/*
* Copyright (c) [2020-2025] NingLeixueR
* 
* ��Ŀ���ƣ�ngl_server
* ��Ŀ��ַ��https://github.com/NingLeixueR/ngl_server
* 
* ���ļ��� ngl_server ��Ŀ��һ���֣���ѭ MIT ��ԴЭ�鷢����
* �����԰���Э��涨����ʹ�á��޸ĺͷַ�����Ŀ��������ҵ��;��
* ���豣��ԭʼ��Ȩ�����������
* 
* �������μ���Ŀ��Ŀ¼�µ� LICENSE �ļ���
* https://github.com/NingLeixueR/ngl_server/blob/main/LICENSE
*/
#pragma once

#include "nserialize.h"
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
		i16_port	m_port = 0;
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
				ngl::ser::serialize_pop lserialize((const char*)lreply->str, lreply->len);
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
		redisContext*	m_rc = nullptr;
		redis_arg		m_arg;
		redis();

		template <typename T>
		bool get(const std::string& aname, int akey, T& adata)
		{
			return redis_cmd::get(m_rc, aname.c_str(), akey, adata);
		}

		template <typename T>
		bool set(const std::string& aname, int akey, T& adata)
		{
			return redis_cmd::set(m_rc, aname.c_str(), akey, adata);
		}

		template <typename T>
		bool set(const std::string& aname, std::map<int, T>& adata)
		{
			for (auto itor = adata.begin(); itor != adata.end(); ++itor)
			{
				if (set(aname, itor->first, itor->second) == false)
				{
					return false;
				}
			}
			return true;
		}
	public:
		redis(const redis_arg& arg);

		template <typename T>
		bool get(int akey, T& adata)
		{
			return get(tools::type_name<T>(), akey, adata);
		}

		template <typename T>
		bool get(std::map<int, T>& adata)
		{
			return redis_cmd::get(m_rc, tools::type_name<T>(), adata);
		}

		template <typename T>
		bool set(int akey, const T& adata)
		{
			return set(tools::type_name<T>(), akey, adata);
		}

		template <typename T>
		bool set(std::map<int, T>& adata)
		{
			return set(tools::type_name<T>(), adata);
		}

		template <typename T>
		bool del(int aid)
		{
			return redis_cmd::del(m_rc, tools::type_name<T>(), aid);
		}
	};

	void test_hiredis();
}// namespace ngl