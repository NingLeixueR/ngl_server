#include "actor_rfun.h"

namespace ngl
{
	arfunbase& arfunbase::set_notfindfun(const tnotfindfun& afun)
	{
		m_notfindfun = afun;
		return *this;
	}

	void arfunbase::notfindfun(actor_base* aactor, i32_threadid athreadid, handle_pram& apram)
	{
		if (m_notfindfun != nullptr)
			m_notfindfun(athreadid, apram);
	}

	bool arfunbase::handle_switch(actor_base* aactor, i32_threadid athreadid, handle_pram& apram)
	{
		auto itor = m_fun.find(apram.m_enum);
		if (itor == m_fun.end())
		{
			if (aactor->type() != ACTOR_ADDRESS_CLIENT)
				LogLocalError("handle_switch m_fun.find(%) == end", apram.m_enum);
			return false;
		}
		if (aactor->isloadfinish() == false && itor->second.m_isdbload == false)
			return false;
		itor->second.m_fun(aactor, athreadid, apram);
		return true;
	}
}