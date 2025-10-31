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
#include "handle_pram.h"
#include "naddress.h"
#include "nguid.h"

namespace ngl
{
	actor_node_session::actor_node_session(i32_sessionid asession, const nactornode& anode) :
		m_session(asession),
		m_node(anode)
	{}

	actor_node_session::actor_node_session(const nactornode& anode) :
		m_node(anode)
	{}

	i32_serverid handle_pram::get_server(i64_actorid aactorid)
	{
		return naddress::get_server(aactorid);
	}

	i32_serverid handle_pram::gatewayid(i64_actorid aactorid)
	{
		return naddress::gatewayid(aactorid);
	}

	void handle_pram::get_serverlist(ENUM_ACTOR atype, std::set<i32_serverid>& avec)
	{
		naddress::get_serverlist(atype, avec);
	}

	bool handle_pram::is_actoridnone(const nguid& aguid)
	{
		return aguid.is_actoridnone();
	}
}//namespace ngl