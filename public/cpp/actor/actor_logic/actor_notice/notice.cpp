#include "actor_notice.h"
#include "notice.h"
#include "net.pb.h"

namespace ngl
{
	std::shared_ptr<pbnet::PROBUFF_NET_NOTICE_RESPONSE> notice::sync_notice(i64_actorid aactorid/* = -1*/)
	{
		auto pro = std::make_shared<pbnet::PROBUFF_NET_NOTICE_RESPONSE>();
		if (aactorid == -1)
		{
			//std::pair<const nguid, data_modified<pbdb::db_notice>>
			for (std::pair<const nguid, data_modified<pbdb::db_notice>>& lpair : data())
			{
				data_modified_continue_getconst(lpdbnotice, lpair.second);
				*pro->add_mnotices() = *lpdbnotice;
			}
			return pro;
		}
		data_modified<pbdb::db_notice>* lpdb_notice = find(aactorid);
		if (lpdb_notice == nullptr)
		{
			return nullptr;
		}
		data_modified_return_getconst(lpdbnotice, *lpdb_notice, nullptr);
		*pro->add_mnotices() = *lpdbnotice;
		return pro;
	}

	void notice::remove_notice()
	{
		int32_t lnow = (int32_t)localtime::gettime();
		for (std::pair<const nguid, data_modified<pbdb::db_notice>>& lpair : data())
		{
			data_modified_continue_getconst(lpnotice, lpair.second);
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