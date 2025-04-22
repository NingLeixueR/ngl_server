#include "actor_kcp.h"
#include "ukcp.h"
namespace ngl
{
	bool actor_kcp::handle(const message<np_actor_kcp>& adata)
	{
		auto lpram = adata.get_data();
		auto lpack = adata.m_pack;
		nets::kcp()->reset_add(lpram->m_conv, lpram->m_uip, lpram->m_uport);

		nets::sendbysession(lpack->m_id, *lpram, nguid::make(), nguid::make());
		return true;
	}
}//namespace ngl
