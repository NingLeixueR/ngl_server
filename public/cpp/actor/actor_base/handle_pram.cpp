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

	i32_serverid handle_pram::serverid(i64_actorid aactorid)
	{
		return naddress::serverid(aactorid);
	}

	i32_serverid handle_pram::gatewayid(i64_actorid aactorid)
	{
		return naddress::gatewayid(aactorid);
	}

	void handle_pram::serveridlist(ENUM_ACTOR atype, std::set<i32_serverid>& avec)
	{
		naddress::serveridlist(atype, avec);
	}

	bool handle_pram::is_actoridnone(const nguid& aguid)
	{
		return aguid.is_actoridnone();
	}
}//namespace ngl