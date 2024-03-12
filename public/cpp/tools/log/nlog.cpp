#include "actor_base.h"
#include "logprintf.h"
#include "nprotocol.h"
#include "xmlnode.h"
#include "nlog.h"

#include <iostream>

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

	void nlog::plog(ELOG atype, ngl::logformat& llogformat, ELOG_TYPE altype)
	{
		if (DEF_LOG_PRINTF)
			logprintf::printf(atype, llogformat.data("pos").c_str(), llogformat.data("head").c_str(), llogformat.data("src").c_str());
		
		if (isinitfinish() == false)
			return;

		std::shared_ptr<ng_actor_logitem> pro(new ng_actor_logitem());
		pro->m_data.m_head.swap(llogformat.data("head"));
		pro->m_data.m_pos.swap(llogformat.data("pos"));
		pro->m_data.m_str.swap(llogformat.data("src"));
		pro->m_data.m_serverid = nconfig::m_nodeid;
		pro->m_data.m_type = atype;

		i64_actorid lactorid = nguid::make(ACTOR_LOG, tab_self_area, altype);
		actor_base::static_send_actor(lactorid, nguid::make(), pro);	
	}
}// namespace ngl