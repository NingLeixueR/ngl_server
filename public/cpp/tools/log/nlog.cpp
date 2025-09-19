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
		{
			return "debug";
		}
		case ELOG_INFO:
		{
			return "info";
		}
		case ELOG_WARN:
		{
			return "warn";
		}
		case ELOG_ERROR:
		{
			return "error";
		}
		}
		return "none";
	}

	std::shared_ptr<nactor_logitem> g_nonelog = std::make_shared<nactor_logitem>();

	std::shared_ptr<nactor_logitem> get_log(const std::source_location& asource, ELOGLEVEL alevel, ELOG_TYPE atype, bool anet)
	{
		if (nactor_logitem::check_level(alevel))
		{
			return std::make_shared<nactor_logitem>(alevel, ACTOR_NONE, atype, anet, asource);
		}
		else
		{
			return g_nonelog;
		}
	}

	std::shared_ptr<nactor_logitem> log_debug(const std::source_location& asource)
	{
		return get_log(asource, ELOG_DEBUG, ELOG_DEFAULT, false);
	}

	std::shared_ptr<nactor_logitem> log_debug_net(const std::source_location& asource)
	{
		return get_log(asource, ELOG_DEBUG, ELOG_DEFAULT, true);
	}
	
	std::shared_ptr<nactor_logitem> log_info(const std::source_location& asource)
	{
		return get_log(asource, ELOG_INFO, ELOG_DEFAULT, false);
	}

	std::shared_ptr<nactor_logitem> log_info_net(const std::source_location& asource)
	{
		return get_log(asource, ELOG_INFO, ELOG_DEFAULT, true);
	}

	std::shared_ptr<nactor_logitem> log_warn(const std::source_location& asource)
	{
		return get_log(asource, ELOG_WARN, ELOG_DEFAULT, false);
	}

	std::shared_ptr<nactor_logitem> log_warn_net(const std::source_location& asource)
	{
		return get_log(asource, ELOG_WARN, ELOG_DEFAULT, true);
	}

	std::shared_ptr<nactor_logitem> log_error(const std::source_location& asource)
	{
		return get_log(asource, ELOG_ERROR, ELOG_DEFAULT, false);
	}

	std::shared_ptr<nactor_logitem> log_error_net(const std::source_location& asource)
	{
		return get_log(asource, ELOG_ERROR, ELOG_DEFAULT, true);
	}

	std::shared_ptr<nactor_logitem> log_bi(const std::source_location& asource)
	{
		return get_log(asource, ELOG_ERROR, ELOG_BI, true);
	}
}// namespace ngl