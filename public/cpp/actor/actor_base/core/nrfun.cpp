/*
* Copyright (c) [2020-2025] NingLeixueR
* 
* Project name: ngl_server
* Project URL: https://github.com/NingLeixueR/ngl_server
* 
* This file is part of the ngl_server project and is distributed under the MIT License.
* You may use, modify, and distribute this project under the license, including commercial use,
* but you must retain the original copyright and license notice.
* 
* For license details, see the LICENSE file in the project root:
* https://github.com/NingLeixueR/ngl_server/blob/main/LICENSE
*/
// File overview: Implements actor dispatch registration tables.

#include "tools/tab/xml/sysconfig.h"
#include "actor/actor_base/core/nrfun.h"
#include "tools/nconsume.h"

namespace ngl
{
	nrfunbase& nrfunbase::set_notfindfun(const tnotfindfun& afun)
	{
		m_notfindfun = afun;
		return *this;
	}

	void nrfunbase::notfindfun(const actor_base*, i32_threadid athreadid, handle_pram& apram)const
	{
		if (m_notfindfun != nullptr)
		{
			m_notfindfun(athreadid, apram);
		}
	}

	bool nrfunbase::handle_switch(actor_base* aactor, i32_threadid athreadid, handle_pram& apram)
	{
		auto lpfun = tools::findmap(m_fun, apram.m_enum);
		if (lpfun == nullptr)
		{
			if (aactor->type() != ACTOR_CLIENT && aactor->type() != ACTOR_SERVER)
			{
				const tprotocol::info* lpinfo = tprotocol::get(apram.m_enum);
				if (lpinfo == nullptr)
				{
					log_error()->print("{}::handle_switch find({}) == end", aactor->guid(), apram.m_enum);
				}
				else
				{
					log_error()->print("{}::handle_switch find({}:{}) == end", aactor->guid(), apram.m_enum, lpinfo->m_name);
				}
			}
			return false;
		}
		if (lpfun->m_ready != e_ready_null && !aactor->ready().is_ready(lpfun->m_ready))
		{
			// The message stays queued at the actor level until the relevant ready bits flip.
			log_error()->print("{}::handle_switch isloadfinish() == {}", aactor->guid(), lpfun->m_ready);
			return false;
		}
		// nconsume measures per-message dispatch latency for diagnostics.
		nconsume lconsuming(std::format("{}-{}-{}", aactor->guid(), apram.m_enum, tprotocol::name(apram.m_enum)));
		lconsuming.start();
		lpfun->m_fun(aactor, athreadid, apram);
		if (aactor->type() != ACTOR_LOG)
		{
			lconsuming.finish();
		}
		return true;
	}

	void nrfunbase::register_logic(i32_protocolnum aprotocol, int32_t aready, const tlogicfun& afun)
	{
		m_fun.insert_or_assign(aprotocol, nlogicfun
			{
				.m_ready = aready,
				.m_fun = afun
			}
		);
	}
}//namespace ngl
