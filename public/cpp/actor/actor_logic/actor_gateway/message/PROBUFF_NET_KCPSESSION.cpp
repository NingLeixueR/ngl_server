#include "actor_gateway_c2g.h"
#include "actor_gateway_g2c.h"
#include "actor_gateway.h"
#include "actor_server.h"
#include "nregister.h"

namespace ngl
{
	bool actor_gateway::handle(const message<pbnet::PROBUFF_NET_KCPSESSION>& adata)
	{
		auto lpram = adata.get_data();
		auto lpack = adata.m_pack;

		/////////////////////////////////////
		// 多robot公用一个tcp连接会有问题
		//gateway_socket* lpstruct = m_info.get(lpack->m_id);
		//if (lpstruct == nullptr)
		//	return true;
		//i16_area larea = lpstruct->m_area;
		//i32_actordataid lactordataid = lpstruct->m_dataid;
		i64_actorid request_actor = lpack->m_head.get_request_actor();
		i16_area larea = nguid::area(request_actor);
		i32_actordataid lactordataid = nguid::actordataid(request_actor);
		///////////////////////////////

		std::string lkcpsession;
		if (ukcp::create_session(nguid::make(nguid::none_type(), larea, lactordataid), lkcpsession) == false)
		{
			return true;
		}

		// ### 通知kcp服务器创建连接
		np_actor_kcp pro;
		pro.m_kcpsession = lkcpsession;
		pro.m_sessionid = lpack->m_id;
		pro.m_area = nguid::area(request_actor);
		pro.m_dataid = nguid::actordataid(request_actor);
		pro.m_uip = lpram->m_uip();
		pro.m_uport = lpram->m_uport();
		pro.m_conv = lpram->m_conv();

		nets::sendbyserver((i32_serverid)lpram->m_serverid(), pro, nguid::make(), nguid::make());

		return true;
	}
}//namespace ngl