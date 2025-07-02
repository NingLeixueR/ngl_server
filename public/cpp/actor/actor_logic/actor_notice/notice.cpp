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
			for (const auto& [id, dbnotice] : data())
			{
				*pro->add_mnotices() = dbnotice.getconst();
			}
			return pro;
		}
		data_modified<pbdb::db_notice>* lpdb_notice = find(aactorid);
		if (lpdb_notice == nullptr)
		{
			return nullptr;
		}
		*pro->add_mnotices() = lpdb_notice->getconst();
		return pro;
	}

	void notice::remove_notice()
	{
		int32_t lnow = (int32_t)localtime::gettime();
		std::map<nguid, data_modified<pbdb::db_notice>>& lnotice = data();
		for (const auto& [id, dbnotice] : lnotice)
		{
			const pbdb::db_notice& lnotice = dbnotice.getconst();
			if (lnotice.mfinishtime() != -1)
				continue;
			if (localtime::checkutc(lnotice.mfinishtime()) == false)
			{
				nactor()->log_error()->print(
					"remove_notice {}:{}-{}:{}"
					, lnotice.mid()
					, localtime::time2str(lnotice.mstarttime(), "%Y-%m-%d %H:%M:%S")
					, localtime::time2str(lnotice.mfinishtime(), "%Y-%m-%d %H:%M:%S")
					, lnotice.mnotice()
				);
				remove(lnotice.mid());
			}
		}
	}
}//namespace ngl