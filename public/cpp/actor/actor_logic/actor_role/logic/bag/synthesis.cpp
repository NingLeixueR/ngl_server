#include "actor_role.h"
#include "synthesis.h"
#include "consume.h"
#include "drop.h"

namespace ngl
{
	bool synthesis::use(actor_role* arole, int32_t aid)
	{
		tab_synthesis* tab = allcsv::tab<tab_synthesis>(aid);
		if (tab == nullptr)
		{
			return false;
		}
		if (consume::use(arole, tab->m_consume, 1) == false)
		{
			log_error()->print("synthesis::use id:{} role:{} consume::use faile {}", aid, arole->id_guid(), tab->m_consume);
			return false;
		}
		if (drop::use(arole, tab->m_generate, 1) == false)
		{
			log_error()->print("synthesis::use id:{} role:{} drop::use faile {}", aid, arole->id_guid(), tab->m_generate);
			return false;
		}
		return true;
	}
}// namespace ngl