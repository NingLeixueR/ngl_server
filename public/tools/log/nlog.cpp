#include "nlog.h"

#include <iostream>
#include "xmlnode.h"
#include "actor_protocol.h"
#include "actor_base.h"
#include "logprintf.h"

namespace ngl
{
	const char* elog_name::get(ELOG atype)
	{
		switch (atype)
		{
		case ELOG_DEBUG:
			return "debug";
		case ELOG_INFO:
			return "info";
		case ELOG_WARN:
			return "warn";
		case ELOG_ERROR:
			return "error";
		}
		return "none";
	}

	bool& nlog::isinitfinish()
	{
		return m_isinitfinish;
	}

	void nlog::plog(ELOG atype, ngl::logformat& llogformat, bool aislocal/* = false*/)
	{
		if (DEF_LOG_PRINTF)
			logprintf::printf(atype, llogformat.data("pos").c_str(), llogformat.data("head").c_str(), llogformat.data("src").c_str());
		
		if (isinitfinish() == false)
			return;

		if (aislocal)
		{
			std::shared_ptr<actor_logitempro> pro(new actor_logitempro());
			pro->m_data.m_head.swap(llogformat.data("head"));
			pro->m_data.m_pos.swap(llogformat.data("pos"));
			pro->m_data.m_str.swap(llogformat.data("src"));
			pro->m_data.m_serverid = nconfig::m_nodeid;
			pro->m_data.m_type = atype;
			actor_base::static_send_actor(actor_guid::make(ACTOR_LOG, tab_self_area, ELOG_LOCAL), actor_guid::make(), pro);
		}
		else
		{
			std::shared_ptr<actor_logitempro> pro(new actor_logitempro());
			pro->m_data.m_head.swap(llogformat.data("head"));
			pro->m_data.m_pos.swap(llogformat.data("pos"));
			pro->m_data.m_str.swap(llogformat.data("src"));
			pro->m_data.m_serverid = nconfig::m_nodeid;
			pro->m_data.m_type = atype;
			actor_base::static_send_actor(actor_guid::make(ACTOR_LOG, tab_self_area, ELOG_DEFAULT), actor_guid::make(), pro);
		}		
	}

	void nlog::plog_bi(ELOG atype, ngl::logformat& llogformat)
	{
		if (DEF_LOG_PRINTF)
			logprintf::printf(atype, llogformat.data("pos").c_str(), llogformat.data("head").c_str(), llogformat.data("src").c_str());
		std::shared_ptr<actor_logitempro> pro(new actor_logitempro());
		pro->m_data.m_head.swap(llogformat.data("head"));
		pro->m_data.m_pos.swap(llogformat.data("pos"));
		pro->m_data.m_str.swap(llogformat.data("src"));
		pro->m_data.m_serverid = nconfig::m_nodeid;
		pro->m_data.m_type = atype;
		actor_base::static_send_actor(actor_guid::make(ACTOR_LOG, tab_self_area, ELOG_BI), actor_guid::make(), pro);
	}
}