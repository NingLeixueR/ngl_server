#include "actor_base.h"
#include "actor_log.h"
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

	nlog::nlog()
	{
		
	}

	void nlog::plog(ELOG atype, ngl::logformat& llogformat, ELOG_TYPE altype)
	{
		if (ngl::nlogsys::console())
			logprintf::printf(atype, llogformat.data("pos").c_str(), llogformat.data("head").c_str(), llogformat.data("src").c_str());
		
		if (isinitfinish() == false)
			return;

		if (nlogsys::iswrite() == false)
			return;

		std::shared_ptr<np_actor_logitem> pro(new np_actor_logitem());
		pro->m_data.m_head.swap(llogformat.data("head"));
		pro->m_data.m_pos.swap(llogformat.data("pos"));
		pro->m_data.m_str.swap(llogformat.data("src"));
		pro->m_data.m_serverid = nconfig::m_nodeid;
		pro->m_data.m_type = atype;

		actor_base::static_send_actor(actor_log::actorid(altype), nguid::make(), pro);
	}

	ELOG	nlogsys::m_level = ELOG_ERROR;
	int32_t	nlogsys::m_line = 10000;
	int32_t	nlogsys::m_flushtime = 10;
	bool	nlogsys::m_iswrite = true;
	bool	nlogsys::m_console = false;

	void nlogsys::init()
	{
		ngl::xmlinfo* lpublicxml = ngl::xmlnode::get_publicconfig();
		lpublicxml->find("log_iswrite", m_iswrite);
		lpublicxml->find("log_flushtime", m_flushtime);
		lpublicxml->find("log_fileline", m_line);
		lpublicxml->find("log_console", m_console);
		int llevel = 0;
		lpublicxml->find("log_level", llevel);
		m_level = (ELOG)llevel;
	}
}// namespace ngl