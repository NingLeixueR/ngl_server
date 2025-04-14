#include "actor_example_manage.h"
#include "actor_example_match.h"
#include "actor_brief.h"

namespace ngl
{
	bool actor_example_match::handle(const message<mforward<pbexample::PROBUFF_EXAMPLE_PLAY_CANCEL>>& adata)
	{
		i64_actorid lroleid = adata.get_data()->identifier();
		int32_t* lproomid = tools::findmap(m_matching, lroleid);
		if (lproomid == nullptr)
		{
			send_error(pbexample::EERROR_CODE_NOTFINDROOM, nullptr, 0, lroleid);
			return true;
		}

		room* lproom = find_room(*lproomid);
		if (lproom == nullptr)
		{
			send_error(pbexample::EERROR_CODE_NOTFINDROOM, nullptr, 0, lroleid);
			return true;
		}

		erase_player_room(lproom, lroleid);
		return true;
	}
}//namespace ngl