/*
* Copyright (c) [2020-2025] NingLeixueR
* 
* Project name: ngl_server
* Project URL: https://github.com/NingLeixueR/ngl_server
* 
* This file is part of the ngl_server project and is distributed under the MIT License.
* You may use, modify, and distribute this project under the license, including commercial use,
* but you must retain the original copyright and license notice.
* 
* For license details, see the LICENSE file in the project root:
* https://github.com/NingLeixueR/ngl_server/blob/main/LICENSE
*/
// File overview: Implements mail data operations and attachment handling.


#include "actor/actor_logic/actor_mail/actor_mail.h"
#include "actor/actor_logic/actor_mail/mail.h"

namespace ngl
{
	bool mails::drawmail(i64_actorid aroleid, int64_t aid)
	{
		if (aid != -1)
		{
			pbdb::mail* ltemp = get_mail(aroleid, aid);
			if (ltemp == nullptr)
			{
				return false;
			}
			if (ltemp->mdraw() == true)
			{
				return false;
			}
			auto pro = std::make_shared<np_actor_senditem>();
			for (const pbdb::mailitem& lmailitem : ltemp->mitems())
			{
				pro->m_item.emplace(lmailitem.mitemtid(), lmailitem.mcount());
			}
			actor::send_actor(aroleid, get_actor()->id_guid(), pro);
		}
		else
		{// Claim
			one_touch(aroleid, [](const pbdb::mail& amail)
				{
					return amail.mdraw() == false;
				}, [aroleid, this](int32_t aid)
				{
					drawmail(aroleid, aid);
				});
		}
		return true;
	}
}//namespace ngl
