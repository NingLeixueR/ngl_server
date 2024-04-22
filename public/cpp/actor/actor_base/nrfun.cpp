#include "nrfun.h"
#include "time_consuming.h"

namespace ngl
{
	const int32_t g_consumings = 100;

	nrfunbase& nrfunbase::set_notfindfun(const tnotfindfun& afun)
	{
		m_notfindfun = afun;
		return *this;
	}

	void nrfunbase::notfindfun(actor_base* aactor, i32_threadid athreadid, handle_pram& apram)
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
				LogLocalError("handle_switch m_fun.find(%) == end", apram.m_enum)
			return false;
		}
		bool lisloadfinish = aactor->isloadfinish();
		if (lisloadfinish == false && itor->second.m_isdbload == false)
		{
			LogLocalError("%::handle_switch isloadfinish() == %", nactortype::enum2name(aactor->type()), lisloadfinish)
			return false;
		}
		if (aactor->type() != ACTOR_LOG)
		{
			time_consuming lconsuming(
				std::format("{}-{}", nactortype::enum2name(aactor->type()), apram.m_enum),
				[](int64_t abeg, int64_t aend)->bool
				{
					return (aend - abeg) > g_consumings;
				});
			itor->second.m_fun(aactor, athreadid, apram);
		}
		else
		{
			itor->second.m_fun(aactor, athreadid, apram);
		}
		return true;
	}
}//namespace ngl