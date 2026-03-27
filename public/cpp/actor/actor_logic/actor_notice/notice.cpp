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
// File overview: Implements logic for actor notice.


#include "actor/actor_logic/actor_notice/actor_notice.h"
#include "actor/actor_logic/actor_notice/notice.h"
#include "actor/generated/pb/net.pb.h"

namespace ngl
{
	bool notice::sync_notice(i64_actorid aactorid, pbnet::PROBUFF_NET_NOTICE_RESPONSE& apro)
	{
		if (aactorid == nguid::make())
		{
			for (auto& lpair : data())
			{
				MODIFIED_CONTINUE_CONST(lpdbnotice, lpair.second);
				*apro.add_mnotices() = *lpdbnotice;
			}
			return true;
		}
		data_modified<pbdb::db_notice>* lpdb_notice = find(aactorid);
		if (lpdb_notice == nullptr)
		{
			return false;
		}
		MODIFIED_RETURN_CONST(lpdbnotice, *lpdb_notice, false);
		*apro.add_mnotices() = *lpdbnotice;
		return true;
	}

	void notice::remove_notice()
	{
		for (auto& lpair : data())
		{
			MODIFIED_CONTINUE_CONST(lpnotice, lpair.second);
			if (lpnotice->mfinishtime() != -1)
			{
				continue;
			}
			if (tools::localtime::checkutc(lpnotice->mfinishtime()) == false)
			{
				log_error()->print(
					"remove_notice {}:{}-{}:{}"
					, lpnotice->mid()
					, tools::localtime::time2str(lpnotice->mstarttime(), "%Y-%m-%d %H:%M:%S")
					, tools::localtime::time2str(lpnotice->mfinishtime(), "%Y-%m-%d %H:%M:%S")
					, lpnotice->mnotice()
				);
				remove(lpnotice->mid());
			}
		}
	}
}//namespace ngl
