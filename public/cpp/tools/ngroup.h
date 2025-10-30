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

#include <map>
#include <set>

namespace ngl
{
	class ngroup
	{
		struct info
		{
			ENUM_ACTOR m_type;
			std::set<i64_actorid> m_actorlist;
		};
		std::map<int, info>				m_group;
		int								m_currentoffset;
	public:
		inline ngroup() :
			m_currentoffset(0)
		{}

		// # ����һ������
		inline int create(ENUM_ACTOR atype = ACTOR_NONE)
		{
			auto& linfo = m_group[++m_currentoffset];
			linfo.m_actorlist.clear();
			linfo.m_type = atype;
			return m_currentoffset;
		}

		// # �Ƴ�һ������
		inline void remove(int agroupid)
		{
			m_group.erase(agroupid);
		}

		// # �ڷ��������һ����Ա
		inline bool add_member(int agroupid, i64_actorid amember)
		{
			info* linfo = tools::findmap(m_group, agroupid);
			if (linfo == nullptr)
			{
				log_error()->print("add_group_member not find groupid[{}]", agroupid);
				return false;
			}
			if (linfo->m_type != ACTOR_NONE)
			{
				amember = nguid::make_type(amember, linfo->m_type);
			}
			linfo->m_actorlist.insert(amember);
			return true;
		}

		// # �ڷ������Ƴ�һ����Ա
		inline void remove_member(int agroupid, i64_actorid amember)
		{
			info* linfo = tools::findmap(m_group, agroupid);
			if (linfo == nullptr)
			{
				return;
			}
			if (linfo->m_type != ACTOR_NONE)
			{
				amember = nguid::make_type(amember, linfo->m_type);
			}
			linfo->m_actorlist.erase(amember);
		}

		// # ��ȡ�����е����г�Ա
		inline const std::set<i64_actorid>* get_group(int agroupid)
		{
			info* linfo = tools::findmap(m_group, agroupid);
			if (linfo == nullptr)
			{
				return nullptr;
			}
			return &linfo->m_actorlist;
		}
	};
}//namespace ngl