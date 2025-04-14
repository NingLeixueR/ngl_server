#include "actor_example_manage.h"
#include "actor_example_match.h"

namespace ngl
{
	bool actor_example_manage::handle(const message<mforward<pbexample::PROBUFF_EXAMPLE_PLAY_ENTER_EXAMPLE>>& adata)
	{
		const pbexample::PROBUFF_EXAMPLE_PLAY_ENTER_EXAMPLE* lpdata = adata.get_data()->data();
		i64_actorid roleid = adata.get_data()->identifier();

		playinfo* lpplayinfo = tools::findmap(m_info[lpdata->m_type()], lpdata->m_exampleactorid());
		if (lpplayinfo == nullptr)
		{
			actor_example_match::send_error(pbexample::EERROR_CODE_NOTFIND, nullptr, 0, roleid);
			return true;
		}

		if (lpplayinfo->m_roles.contains(roleid) == false)
		{
			actor_example_match::send_error(pbexample::EERROR_CODE_NOTFIND, nullptr, 0, roleid);
			return true;
		}

		enter_game(lpplayinfo, roleid, lpdata->m_cross(), lpdata->m_type());

		return true;
	}
}//namespace ngl