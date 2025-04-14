#include "actor_login.h"
#include "nregister.h"

namespace ngl
{
	bool actor_login::handle(const message<pbnet::PROBUFF_NET_ACOUNT_LOGIN>& adata)
	{
		Try
		{
			auto lparm = adata.get_data();
			auto lpack = adata.m_pack;
			Assert(lpack != nullptr)
			log_info()->print("############ Login[{}][{}][{}] ############",
				lparm->m_area(), lparm->m_account(), lparm->m_password()
			);
			bool iscreate = false;

			const data_modified<pbdb::db_account>* lpaccount = get_account(lparm->m_area(), lparm->m_account(), lparm->m_password(), iscreate);
			Assert(lpaccount != nullptr)

			const pair_account* lppair_account = nullptr;
			auto itor = m_actorbyserver.find(lpaccount->getconst().m_id());
			if (itor == m_actorbyserver.end())
			{
				pair_account ltempaccount;
				// 生成session key
				tools::uuid_make(ltempaccount.m_session);

				std::pair<int32_t, int32_t> lpairgame;
				std::pair<int32_t, int32_t> lpairgateway;
				Assert(get_freeserver_game(lpairgame))
				Assert(get_freeserver_gateway(lpairgateway))
				ltempaccount.m_gameserverid = lpairgame.first;
				ltempaccount.m_gatewayserverid = lpairgateway.first;
				m_actorbyserver[lpaccount->getconst().m_id()] = ltempaccount;
				lppair_account = &m_actorbyserver[lpaccount->getconst().m_id()];
			}
			else
			{
				lppair_account = &itor->second;
			}

			// 通知gateway服务器				
			np_actorrole_login pro
			{
				.m_session = lppair_account->m_session,
				.m_accountid = lpaccount->getconst().m_id(),
				.m_account = lparm->m_account(),
				.m_roleid = lpaccount->getconst().m_roleid(),
				.m_gameid = lppair_account->m_gameserverid,
				.m_gatewayid = lppair_account->m_gatewayserverid,
				.m_area = (int16_t)lpaccount->getconst().m_area(),
				.m_iscreate = iscreate,
				.m_socketid = adata.m_pack->m_id,
				.m_request_actor = lpack->m_head.get_request_actor(),
			};
			nets::sendbyserver(pro.m_gatewayid, pro, nguid::moreactor(), id_guid());

			pbnet::PROBUFF_NET_ACOUNT_LOGIN_RESPONSE procli;
			procli.set_m_roleid(pro.m_roleid);
			procli.set_m_area(pro.m_area);
			procli.set_m_session(pro.m_session);
			procli.set_m_account(pro.m_account);
			procli.set_m_gatewayid(pro.m_gatewayid);
			nets::sendbysession(adata.m_pack->m_id, procli, lpack->m_head.get_request_actor(), id_guid());

		}Catch
		return true;
	}
}//namespace ngl