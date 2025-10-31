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
#include "server_session.h"
#include "ttab_servers.h"
#include "nprotocol.h"
#include "nlog.h"

namespace ngl
{
	std::multimap<i32_serverid, i32_sessionid> server_session::m_server;
	std::multimap<i32_sessionid, i32_serverid> server_session::m_session;
	std::shared_mutex server_session::m_mutex;

	void server_session::add(i32_serverid aserverid, i32_sessionid asession)
	{
		//log_error()->print("server_session::add [{}:{}] [{}]", aserverid, ttab_servers::instance().tab(nnodeid::tid(aserverid))->m_name, asession);
		{
			lock_write(m_mutex);
			m_server.erase(aserverid);
			m_session.erase(asession);
			m_server.insert(std::make_pair(aserverid, asession));
			m_session.insert(std::make_pair(asession, aserverid));
		}
	}

	void server_session::remove(i32_sessionid asession)
	{
		lock_write(m_mutex);
		auto itor = m_session.find(asession);
		if (itor == m_session.end())
		{
			return;
		}
		
		log_error()->print("server_session::remove [{}:{}] [{}]", itor->second, ttab_servers::instance().tab(nnodeid::tid(itor->second))->m_name, asession);
		m_server.erase(itor->second);
		m_session.erase(itor);
	}

	i32_sessionid server_session::sessionid(i32_serverid aserverid)
	{
		lock_read(m_mutex);
		auto itor = m_server.find(aserverid);
		if (itor == m_server.end())
		{
			return -1;
		}
		return itor->second;
	}

	i32_serverid server_session::serverid(i32_sessionid asessionid)
	{
		lock_read(m_mutex);
		auto itor = m_session.find(asessionid);
		if (itor == m_session.end())
		{
			return -1;
		}
		return itor->second;
	}

	bool server_session::serverinfo(i32_serverid aserverid, str_servername& asername)
	{
		if (aserverid != -1)
		{
			const tab_servers* tab = ttab_servers::instance().tab(nnodeid::tid(aserverid));
			if (tab != nullptr)
			{
				asername = tab->m_name;
				return true;
			}
		}
		return false;
	}

	bool server_session::serverinfobysession(i32_sessionid asessionid, std::pair<str_servername, i32_serverid>& apair)
	{
		apair.second = serverid(asessionid);
		if (apair.second == -1)
		{
			return false;
		}
		if (serverinfo(apair.second, apair.first) == false)
		{
			return false;
		}
		return true;
	}
}//namespace ngl