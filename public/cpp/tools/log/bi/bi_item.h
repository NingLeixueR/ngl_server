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

#include "nprotocol.h"

namespace ngl
{
	class bi
	{
		template <typename T>
		static void add(std::string& astr, const T& avalue)
		{
			if (!astr.empty())
			{
				astr += '|';
			}
			astr += tools::lexical_cast<std::string>(avalue);
		}

		template <typename T, typename ...ARGS>
		static void add(std::string& astr, const T& avalue, const ARGS&... args)
		{
			push(astr, avalue);
			push(astr, args...);
		}

	public:
		template <typename T, typename ...ARGS>
		static std::string push(const T& avalue, const ARGS&... args)
		{
			std::string lstr;
			push(lstr, args...);
			return lstr;
		}
	};

	struct bi_item
	{
		i32_serverid m_serverid;
		i64_actorid m_roleid;
		int32_t m_itemid;
		int32_t m_tid;
		int32_t m_count;
		int32_t m_time;
		int32_t m_adddec;// 1:���� 2:����
		std::string m_mask;

		void print()
		{
			log_bi()->print("item|{}", bi::push(m_serverid, m_roleid, m_itemid, m_tid, m_count, m_time, m_adddec, m_mask));
		}
	};


}