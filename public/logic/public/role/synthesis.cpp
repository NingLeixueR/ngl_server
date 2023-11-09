#include "synthesis.h"
#include "actor_role.h"
#include "consume.h"
#include "drop.h"

namespace ngl
{
	bool synthesis::use(actor_role* arole, int32_t aid)
	{
		tab_synthesis* tab = allcsv::tab<tab_synthesis>(aid);
		if (tab == nullptr)
			return false;
		if (consume::check(arole, tab->m_consume, 1) == false)
			return false;
		consume::use(arole, tab->m_consume, 1);
		drop::use(arole, tab->m_generate, 1);
		return true;
	}


}