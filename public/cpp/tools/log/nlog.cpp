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

	std::shared_ptr<np_actor_logitem> get_log(const std::source_location& asource, ELOGLEVEL alevel, bool anet)
	{
		if (alevel >= ngl::sysconfig::loglevel())
		{
			return std::make_shared<np_actor_logitem>(alevel, ACTOR_NONE, anet? ELOG_NETWORK:ELOG_LOCAL, asource);
		}
		else
		{
			return g_nonelog;
		}
	}

	std::shared_ptr<np_actor_logitem> log_debug(const std::source_location& asource)
	{
		return get_log(asource, ELOG_DEBUG, false);
	}

	std::shared_ptr<np_actor_logitem> log_debug_net(const std::source_location& asource)
	{
		return get_log(asource, ELOG_DEBUG, true);
	}
	
	std::shared_ptr<np_actor_logitem> log_info(const std::source_location& asource)
	{
		return get_log(asource, ELOG_INFO, false);
	}

	std::shared_ptr<np_actor_logitem> log_info_net(const std::source_location& asource)
	{
		return get_log(asource, ELOG_INFO, true);
	}

	std::shared_ptr<np_actor_logitem> log_warn(const std::source_location& asource)
	{
		return get_log(asource, ELOG_WARN, false);
	}

	std::shared_ptr<np_actor_logitem> log_warn_net(const std::source_location& asource)
	{
		return get_log(asource, ELOG_WARN, true);
	}

	std::shared_ptr<np_actor_logitem> log_error(const std::source_location& asource)
	{
		return get_log(asource, ELOG_ERROR, false);
	}

	std::shared_ptr<np_actor_logitem> log_error_net(const std::source_location& asource)
	{
		return get_log(asource, ELOG_ERROR, true);
	}
}// namespace ngl