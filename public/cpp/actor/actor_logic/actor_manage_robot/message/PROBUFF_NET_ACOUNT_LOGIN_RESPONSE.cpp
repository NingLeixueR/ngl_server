#include "actor_manage_robot.h"

namespace ngl
{
	bool actor_manage_robot::handle(const message<pbnet::PROBUFF_NET_ACOUNT_LOGIN_RESPONSE>& adata)
	{
		auto lrecv = adata.get_data();
		_robot& lrobot = m_maprobot[lrecv->m_account()];
		lrobot.m_robot = create((i16_area)lrecv->m_area(), nguid::actordataid(lrecv->m_roleid()));
		lrobot.m_account = lrecv->m_account();
		lrobot.m_actor_roleid = nguid::make_type(lrobot.m_robot->id_guid(), ACTOR_ROLE);

		connect(lrecv->m_gatewayid(), [lrecv, &lrobot, this](int asession)
			{
				lrobot.m_session = asession;
				lrobot.m_robot->m_session = asession;
				pbnet::PROBUFF_NET_ROLE_LOGIN pro;
				pro.set_m_roleid(lrecv->m_roleid());
				pro.set_m_session(lrecv->m_session());
				pro.set_m_area(lrecv->m_area());
				pro.set_m_iscreate(false);
				pro.set_m_gatewayid(lrecv->m_gatewayid());
				nets::sendbysession(asession, pro, nguid::moreactor(), this->id_guid());
			});

		return true;
	}
}//namespace ngl