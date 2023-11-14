#include "autoitem.h"
#include "actor_role.h"

namespace ngl
{
	void autoitem::add(int32_t atid, int32_t acount)
	{
		m_addstackitems[atid] += acount;
	}

	void autoitem::add(int32_t aid)
	{
		m_addnostackitems.push_back(aid);
	}

	void autoitem::del(int32_t aid, int32_t acount)
	{
		m_delstackitems[aid] += acount;
	}

	void autoitem::del(int32_t aid)
	{
		m_addnostackitems.push_back(aid);
	}

}