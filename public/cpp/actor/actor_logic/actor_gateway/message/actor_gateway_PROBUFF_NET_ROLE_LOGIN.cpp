#include "actor_gateway_c2g.h"
#include "actor_gateway_g2c.h"
#include "actor_gateway.h"
#include "actor_server.h"
#include "nregister.h"

namespace ngl
{
	bool actor_gateway::handle(const message<pbnet::PROBUFF_NET_ROLE_LOGIN>& adata)
	{
		Try
		{
			auto lpram = adata.get_data();
			auto lpack = adata.m_pack;
			Assert(lpack != nullptr)
			log_info()->print("############ GateWay Login[{}][{}][{}] ############"
				, lpack->m_id, lpram->m_roleid(), lpram->m_session()
			);
			nguid lguid(lpram->m_roleid());
			gateway_socket* linfo = m_info.get(lguid.area(), lguid.actordataid());

			Assert(linfo != nullptr)
			Assert(linfo->m_session == lpram->m_session())

			if (sysconfig::robot_test() == false && lpack->m_id != linfo->m_socket && linfo->m_socket > 0)
			{
				nets::net(linfo->m_socket)->close_net(linfo->m_socket);
				linfo->m_socket = 0;
				if (m_info.updata_socket(lguid.area(), lguid.actordataid(), lpack->m_id))
				{
					update_gateway_info(std::make_shared<np_actor_gatewayinfo_updata>(np_actor_gatewayinfo_updata{.m_add = {*linfo} }));
				}
				// 断线重连或者其他设备顶号
				pbnet::PROBUFF_NET_ROLE_SYNC pro;
				i64_actorid lroleactor = nguid::make(ACTOR_ROLE, lguid.area(), lguid.actordataid());
				nets::sendbyserver(linfo->m_gameid, pro, lroleactor, id_guid());
				return true;
			}

			if (m_info.updata_socket(lguid.area(), lguid.actordataid(), lpack->m_id))
			{
				update_gateway_info(std::make_shared<np_actor_gatewayinfo_updata>(np_actor_gatewayinfo_updata{.m_add = {*linfo} }));
			}

			pbnet::PROBUFF_NET_ROLE_LOGIN lprampro = *lpram;
			lprampro.set_m_iscreate(linfo->m_iscreate);
			linfo->m_iscreate = false;
			lprampro.set_m_gatewayid(nconfig::m_nodeid);
			lprampro.set_m_area(linfo->m_area);
			nets::sendbyserver(linfo->m_gameid, *lpram, nguid::moreactor(), id_guid());
			return true;
		}Catch
			return false;
	}
}//namespace ngl