#include "notice.h"
#include "net.pb.h"

namespace ngl
{
	std::shared_ptr<pbnet::PROBUFF_NET_NOTICE_RESPONSE> notice::sync_notice(i64_actorid aactorid/* = -1*/)
	{
		auto pro = std::make_shared<pbnet::PROBUFF_NET_NOTICE_RESPONSE>();
		if (aactorid == -1)
		{
			for (auto&& [id, dbnotice] : *get_notice())
			{
				*pro->add_m_notices() = dbnotice.getconst();
			}
			return pro;
		}
		auto itor = get_notice()->find(aactorid);
		if (itor == get_notice()->end())
			return nullptr;
		
		*pro->add_m_notices() = itor->second.getconst();
		return pro;
	}
}//namespace ngl