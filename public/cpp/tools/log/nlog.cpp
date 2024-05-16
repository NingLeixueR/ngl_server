#include "actor_base.h"
#include "actor_log.h"
#include "logprintf.h"
#include "nprotocol.h"
#include "xmlnode.h"
#include "nlog.h"

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

	std::shared_ptr<np_actor_logitem> log(const std::source_location& asource/* = std::source_location::current()*/)
	{
		return std::make_shared<np_actor_logitem>(ACTOR_NONE, ELOG_LOCAL, asource);
	}

	std::shared_ptr<np_actor_logitem> lognet(const std::source_location& asource/* = std::source_location::current()*/)
	{
		return std::make_shared<np_actor_logitem>(ACTOR_NONE, ELOG_NETWORK, asource);
	}

}// namespace ngl