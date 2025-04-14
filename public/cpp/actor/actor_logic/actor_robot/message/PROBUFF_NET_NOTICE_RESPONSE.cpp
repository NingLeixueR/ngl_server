#include "actor_robot.h"
#include "nregister.h"
#include "nforward.h"

namespace ngl
{
	bool actor_robot::handle(const message<pbnet::PROBUFF_NET_NOTICE_RESPONSE>& adata)
	{
		struct noticeitem
		{
			int64_t		m_id;			// 公告id 
			std::string m_notice;		// 内容
			std::string m_starttime;	// 开始时间
			std::string m_finishtime;	// 结束时间
			jsonfunc("id", m_id, "notice", m_notice, "starttime", m_starttime, "finishtime", m_finishtime)
		};
		struct notices
		{
			std::vector<noticeitem> m_notices;
			jsonfunc("notices", m_notices)
		};
		json_write ljson;
		notices lnotices;
		char lbuffstart[1024] = { 0 };
		char lbufffinish[1024] = { 0 };
		for (const auto& item : adata.get_data()->m_notices())
		{
			ngl::localtime::time2str(lbuffstart, 1024, item.m_starttime(), "%Y/%m/%d %H:%M:%S");
			ngl::localtime::time2str(lbufffinish, 1024, item.m_finishtime(), "%Y/%m/%d %H:%M:%S");
			lnotices.m_notices.push_back(
				noticeitem
				{
					.m_id = item.m_id(),
					.m_notice = item.m_notice(),
					.m_starttime = lbuffstart,
					.m_finishtime = lbufffinish,
				}
				);
		}
		ljson.write("notice_list", lnotices);
		std::string lstr;
		ljson.get(lstr);
		std::string lstrasscii;
		ngl::tools::to_asscii(lstr, lstrasscii);
		log_error()->print(
			"[{}:{}] {}",
			area(), m_data.m_role().m_base().m_name(), lstrasscii
		);
		return true;
	}
}//namespace ngl