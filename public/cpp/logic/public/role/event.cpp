#include "event.h"
#include "map.h"
#include "nlog.h"

namespace ngl
{
	void events::init()
	{
		// ## 死亡
		push<event_parm_death>(enum_event_death, [](event_parm_death* apram)
			{
				log()->error("#enum_event_death unitid:{} deathunitid:{}", apram->m_unitid, apram->m_deathunitid);
			});
		// ## 进入视野(enterview)
		push<event_parm_enterview>(enum_event_enterview, [](event_parm_enterview* apram)
			{
				auto lstream = log();
				(*lstream) << "#enum_event_enterview enterunitid:" << apram->m_enterunitid;
				(*lstream) << "enterunitid:[";
				for (auto itor = apram->m_unitids.begin(); itor != apram->m_unitids.end(); ++itor)
				{
					if (itor != apram->m_unitids.begin())
						(*lstream) << ',';
					(*lstream) << *itor;
				}
				(*lstream) << "]";
				(*lstream).error("");
			});
		// ## 离开视野(leaveview)
		push<event_parm_leaveview>(enum_event_leaveview, [](event_parm_leaveview* apram)
			{
				auto lstream = log();
				(*lstream) << "#enum_event_leaveview enterunitid:" << apram->m_leaveunitid;
				(*lstream) << "enterunitid:[";
				for (auto itor = apram->m_unitids.begin(); itor != apram->m_unitids.end(); ++itor)
				{
					if (itor != apram->m_unitids.begin())
						(*lstream) << ',';
					(*lstream) << *itor;
				}
				(*lstream) << "]";
				(*lstream).error("");
			});
	}
}