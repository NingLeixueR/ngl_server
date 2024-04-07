#include "naddress.h"
#include "nguid.h"
#include "handle_pram.h"

namespace ngl
{
	actor_node_session::actor_node_session() :
		m_session(-1)
	{}

	actor_node_session::actor_node_session(i32_sessionid asession, const nactornode& anode) :
		m_session(asession),
		m_node(anode)
	{}

	actor_node_session::actor_node_session(const nactornode& anode) :
		m_session(-1),
		m_node(anode)
	{}

	i32_serverid handle_pram::get_server(i64_actorid aactorid)
	{
		return naddress::get_server(aactorid);
	}

	i32_serverid handle_pram::get_gatewayid(i64_actorid aactorid)
	{
		return naddress::get_gatewayid(aactorid);
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