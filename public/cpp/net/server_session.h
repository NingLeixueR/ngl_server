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

#include "type.h"

#include <shared_mutex>
#include <map>

namespace ngl
{
	class server_session
	{
		server_session() = delete;
		server_session(const server_session&) = delete;
		server_session& operator=(const server_session&) = delete;

		static std::multimap<i32_serverid, i32_sessionid>	m_server;	// key:serverid value:sessionid
		static std::multimap<i32_sessionid, i32_serverid>	m_session;	// key:sessionid value:serverid
		static std::shared_mutex							m_mutex;
	public:
		// # ��ӷ�����id��sessionidӳ��
		static void				add(i32_serverid aserverid, i32_sessionid asession);

		// # �Ƴ�sessionid
		static void				remove(i32_sessionid asession);

		// # ���ݷ�����id��ȡsessionid
		static i32_sessionid	sessionid(i32_serverid aserverid);

		// # ����sessionid��ȡ������id
		static i32_serverid		serverid(i32_sessionid asessionid);

		// # ����serverid��ȡtab_servers���еķ���������
		static bool				serverinfo(i32_serverid aserverid, str_servername& asername);

		// # ����session��ȡ������id�����������
		static bool				serverinfobysession(i32_sessionid asessionid, std::pair<str_servername, i32_serverid>& apair);
	};
}//namespace ngl