#include "actor_example_manage.h"
#include "actor_example_match.h"
#include "actor_brief.h"

namespace ngl
{
	bool actor_example_match::handle(const message<np_login_request_info>& adata)
	{
		i64_actorid lroleid = adata.get_data()->m_roleid;
		int32_t* lproomid = tools::findmap(m_matching, lroleid);
		if (lproomid == nullptr)
		{
			return true;
		}
		room* lproom = find_room(*lproomid);
		if (lproom == nullptr)
		{
			m_matching.erase(lroleid);
			return true;
		}
		sync_match_info(lproom, adata.get_data()->m_roleid);
		return true;
	}
}//namespace ngl