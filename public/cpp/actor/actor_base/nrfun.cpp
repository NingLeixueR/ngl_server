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
#include "nconsuming.h"
#include "sysconfig.h"
#include "nrfun.h"

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
					log_error()->print("{}::handle_switch  m_fun.find({}) == end", aactor->guid(), apram.m_enum);
				}
				else
				{
					log_error()->print("{}::handle_switch  m_fun.find({}:{}) == end", aactor->guid(), apram.m_enum, lpinfo->m_name);
				}
			}
			return false;
		}
		if (lpfun->m_ready != e_ready_null && !aactor->ready().is_ready(lpfun->m_ready))
		{
			log_error()->print("{}::handle_switch isloadfinish() == {}", aactor->guid(), lpfun->m_ready);
			return false;
		}
		nconsuming lconsuming(std::format("{}-{}-{}", aactor->guid(), apram.m_enum, tprotocol::name(apram.m_enum)));
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
		m_fun[aprotocol] = nlogicfun
		{
			.m_ready = aready,
			.m_fun = afun
		};
	}
}//namespace ngl