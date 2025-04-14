#include "actor_role.h"
#include "ttab_task.h"
#include "drop.h"

namespace ngl
{
	bool actor_role::handle(const message<pbnet::PROBUFF_NET_TASK_RECEIVE_AWARD>& adata)
	{
		tab_task* tab = ttab_task::tab(adata.get_data()->m_taskid());
		if (tab == nullptr)
		{
			return true;
		}

		auto pro = std::make_shared<pbnet::PROBUFF_NET_TASK_RECEIVE_AWARD_RESPONSE>();
		pro->set_m_taskid(adata.get_data()->m_taskid());

		std::map<int, int> ldrop;
		if (drop::droplist(tab->m_dropid, 1, ldrop) == false)
		{
			log_error()->print("task:{} drop:{} fail!!!", adata.get_data()->m_taskid(), tab->m_dropid);
			return true;
		}
		tools::copy(ldrop, *pro->mutable_m_drop());
		m_bag.add_item(ldrop);
		return true;
	}
}//namespace ngl