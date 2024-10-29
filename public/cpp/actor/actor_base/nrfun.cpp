#include "time_consuming.h"
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
			m_notfindfun(athreadid, apram);
	}

	bool nrfunbase::handle_switch(actor_base* aactor, i32_threadid athreadid, handle_pram& apram)
	{
		auto itor = m_fun.find(apram.m_enum);
		if (itor == m_fun.end())
		{
			if (aactor->type() != ACTOR_CLIENT)
			{
				log_error()->print("{}::handle_switch  m_fun.find({}) == end", aactor->guid(), apram.m_enum);
			}
			return false;
		}
		bool lisloadfinish = aactor->isloadfinish();
		if (lisloadfinish == false && itor->second.m_isdbload == false)
		{
			log_error()->print("{}::handle_switch isloadfinish() == {}", aactor->guid(), lisloadfinish);
			return false;
		}
		time_consuming lconsuming(std::format("{}-{}", aactor->guid(), apram.m_enum));
		lconsuming.consuming_start();
		itor->second.m_fun(aactor, athreadid, apram);
		if (aactor->type() != ACTOR_LOG)
		{
			lconsuming.consuming_finish();
		}
		return true;
	}
}//namespace ngl