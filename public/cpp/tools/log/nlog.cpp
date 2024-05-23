#include "actor_base.h"
#include "actor_log.h"
#include "logprintf.h"
#include "nprotocol.h"
#include "nlog.h"
#include "xml.h"

#include <iostream>

namespace ngl
{
	const char* elog_name::get(ELOGLEVEL atype)
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

	std::shared_ptr<np_actor_logitem> g_nonelog = std::make_shared<np_actor_logitem>();
	std::shared_ptr<np_actor_logitem> log_debug(const std::source_location& asource)
	{
		if (ELOG_DEBUG >= ngl::sysconfig::loglevel())
		{
			return std::make_shared<np_actor_logitem>(ELOG_DEBUG, ACTOR_NONE, ELOG_LOCAL, asource);
		}
		else
		{
			return g_nonelog;
		}
	}

	std::shared_ptr<np_actor_logitem> log_debug_net(const std::source_location& asource)
	{
		if (ELOG_DEBUG >= ngl::sysconfig::loglevel())
		{
			return std::make_shared<np_actor_logitem>(ELOG_DEBUG, ACTOR_NONE, ELOG_NETWORK, asource);
		}
		else
		{
			return g_nonelog;
		}
	}
	
	std::shared_ptr<np_actor_logitem> log_info(const std::source_location& asource)
	{
		if (ELOG_INFO >= ngl::sysconfig::loglevel())
		{
			return std::make_shared<np_actor_logitem>(ELOG_INFO, ACTOR_NONE, ELOG_LOCAL, asource);
		}
		else
		{
			return g_nonelog;
		}
	}

	std::shared_ptr<np_actor_logitem> log_info_net(const std::source_location& asource)
	{
		if (ELOG_INFO >= ngl::sysconfig::loglevel())
		{
			return std::make_shared<np_actor_logitem>(ELOG_INFO, ACTOR_NONE, ELOG_NETWORK, asource);
		}
		else
		{
			return g_nonelog;
		}
	}

	std::shared_ptr<np_actor_logitem> log_warn(const std::source_location& asource)
	{
		if (ELOG_WARN >= ngl::sysconfig::loglevel())
		{
			return std::make_shared<np_actor_logitem>(ELOG_WARN, ACTOR_NONE, ELOG_LOCAL, asource);
		}
		else
		{
			return g_nonelog;
		}
	}

	std::shared_ptr<np_actor_logitem> log_warn_net(const std::source_location& asource)
	{
		if (ELOG_WARN >= ngl::sysconfig::loglevel())
		{
			return std::make_shared<np_actor_logitem>(ELOG_WARN, ACTOR_NONE, ELOG_NETWORK, asource);
		}
		else
		{
			return g_nonelog;
		}
	}

	std::shared_ptr<np_actor_logitem> log_error(const std::source_location& asource)
	{
		if (ELOG_ERROR >= ngl::sysconfig::loglevel())
		{
			return std::make_shared<np_actor_logitem>(ELOG_ERROR, ACTOR_NONE, ELOG_LOCAL, asource);
		}
		else
		{
			return g_nonelog;
		}
	}

	std::shared_ptr<np_actor_logitem> log_error_net(const std::source_location& asource)
	{
		if (ELOG_ERROR >= ngl::sysconfig::loglevel())
		{
			return std::make_shared<np_actor_logitem>(ELOG_ERROR, ACTOR_NONE, ELOG_NETWORK, asource);
		}
		else
		{
			return g_nonelog;
		}
	}


}// namespace ngl