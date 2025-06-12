#include "ttab_servers.h"
#include "nguid.h"

namespace ngl
{
	i64_actorid nguid::make_self(ENUM_ACTOR atype)
	{
		nguid lguid(atype, tab_self_area, none_actordataid());
		return (i64_actorid)lguid;
	}

	int16_t nlogactor::m_localnum = 0;

	nlogactor::nlogactor(ENUM_ACTOR avalue1, ELOG_TYPE avalue2) :
		m_value32(0)
	{
		m_value16[0] = avalue1;
		if (avalue2 == ELOG_TYPE::ELOG_LOCAL)
		{
			if (m_localnum == 0)
			{
				m_localnum = (ttab_servers::instance().tab()->m_type * 100 + ttab_servers::instance().tab()->m_tcount);
			}
			m_value16[1] = 0x8000 | m_localnum;
		}
		else
		{
			m_value16[1] = avalue2;
		}
	}
}//namespace ngl