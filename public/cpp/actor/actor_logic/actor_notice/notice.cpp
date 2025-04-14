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
			for (const auto& [id, dbnotice] : *get_notice())
			{
				*pro->add_m_notices() = dbnotice.getconst();
			}
			return pro;
		}
		auto itor = get_notice()->find(aactorid);
		if (itor == get_notice()->end())
		{
			return nullptr;
		}
		
		*pro->add_m_notices() = itor->second.getconst();
		return pro;
	}

	void notice::remove_notice()
	{
		int32_t lnow = (int32_t)localtime::gettime();
		std::map<nguid, data_modified<pbdb::db_notice>>& lnotice = data();
		for (const auto& [id, dbnotice] : lnotice)
		{
			const pbdb::db_notice& lnotice = dbnotice.getconst();
			if (lnotice.m_finishtime() != -1)
				continue;
			if (localtime::checkutc(lnotice.m_finishtime()) == false)
			{
				actor()->log_error()->print(
					"remove_notice {}:{}-{}:{}",
					lnotice.m_id(),
					localtime::time2str(lnotice.m_starttime(), "%Y-%m-%d %H:%M:%S"),
					localtime::time2str(lnotice.m_finishtime(), "%Y-%m-%d %H:%M:%S"),
					lnotice.m_notice()
				);
				remove(lnotice.m_id());
			}
		}
	}
}//namespace ngl