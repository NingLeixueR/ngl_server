/*
* Copyright (c) [2020-2025] NingLeixueR
* 
* 项目名称：ngl_server
* 项目地址：https://github.com/NingLeixueR/ngl_server
* 
* 本文件是 ngl_server 项目的一部分，遵循 MIT 开源协议发布。
* 您可以按照协议规定自由使用、修改和分发本项目，包括商业用途，
* 但需保留原始版权和许可声明。
* 
* 许可详情参见项目根目录下的 LICENSE 文件：
* https://github.com/NingLeixueR/ngl_server/blob/main/LICENSE
*/
#include "actor_notice.h"
#include "notice.h"
#include "net.pb.h"

namespace ngl
{
	bool notice::sync_notice(i64_actorid aactorid, pbnet::PROBUFF_NET_NOTICE_RESPONSE& apro)
	{
		pbnet::PROBUFF_NET_NOTICE_RESPONSE pro;
		if (aactorid == nguid::make())
		{
			for (auto& [_guid, _modified] : data())
			{
				MODIFIED_CONTINUE_CONST(lpdbnotice, _modified);
				*pro.add_mnotices() = *lpdbnotice;
			}
			return true;
		}
		data_modified<pbdb::db_notice>* lpdb_notice = find(aactorid);
		if (lpdb_notice == nullptr)
		{
			return false;
		}
		MODIFIED_RETURN_CONST(lpdbnotice, *lpdb_notice, false);
		*pro.add_mnotices() = *lpdbnotice;
		return true;
	}

	void notice::remove_notice()
	{
		int32_t lnow = (int32_t)localtime::gettime();
		for (auto& [_guid, _modified] : data())
		{
			MODIFIED_CONTINUE_CONST(lpnotice, _modified);
			if (lpnotice->mfinishtime() != -1)
			{
				continue;
			}
			if (localtime::checkutc(lpnotice->mfinishtime()) == false)
			{
				log_error()->print(
					"remove_notice {}:{}-{}:{}"
					, lpnotice->mid()
					, localtime::time2str(lpnotice->mstarttime(), "%Y-%m-%d %H:%M:%S")
					, localtime::time2str(lpnotice->mfinishtime(), "%Y-%m-%d %H:%M:%S")
					, lpnotice->mnotice()
				);
				remove(lpnotice->mid());
			}
		}
	}
}//namespace ngl