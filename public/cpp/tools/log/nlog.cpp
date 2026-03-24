/*
* Copyright (c) [2020-2025] NingLeixueR
* 
* Project name: ngl_server
* Project URL: https://github.com/NingLeixueR/ngl_server
* 
* This file is part of the ngl_server project and is distributed under the MIT License.
* You may use, modify, and distribute this project under the license, including commercial use,
* but you must retain the original copyright and license notice.
* 
* For license details, see the LICENSE file in the project root:
* https://github.com/NingLeixueR/ngl_server/blob/main/LICENSE
*/
// File overview: Implements logic for log.


#include "actor/actor_logic/actor_log/actor_log.h"
#include "actor/actor_base/actor_base.h"
#include "actor/protocol/nprotocol.h"
#include "tools/log/logprintf.h"
#include "tools/tab/xml/xml.h"
#include "tools/log/nlog.h"

#include <iostream>

namespace ngl
{
	const char* elog_name::get(ELOGLEVEL atype)
	{
		switch (atype)
		{
			case ELOG_NONE:
			{
				return "none";
			}
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
			case ELOG_MAX:
			default:
			{
				return "none";
			}
		}
	}

	ptr_logitem g_nonelog = std::make_shared<nactor_logitem>();

	ptr_logitem get_log(const std::source_location& asource, ELOGLEVEL alevel, ELOG_TYPE atype, bool anet)
	{
		// When the level is filtered out, return a shared no-op log item so
		// call sites can keep the same fluent API without branching.
		if (nactor_logitem::check_level(alevel))
		{
			return std::make_shared<nactor_logitem>(alevel, ACTOR_NONE, atype, anet, asource);
		}
		else
		{
			return g_nonelog;
		}
	}

	ptr_logitem log_debug(const std::source_location& asource)
	{
		return get_log(asource, ELOG_DEBUG, ELOG_DEFAULT, false);
	}

	ptr_logitem log_debug_net(const std::source_location& asource)
	{
		return get_log(asource, ELOG_DEBUG, ELOG_DEFAULT, true);
	}
	
	ptr_logitem log_info(const std::source_location& asource)
	{
		return get_log(asource, ELOG_INFO, ELOG_DEFAULT, false);
	}

	ptr_logitem log_info_net(const std::source_location& asource)
	{
		return get_log(asource, ELOG_INFO, ELOG_DEFAULT, true);
	}

	ptr_logitem log_warn(const std::source_location& asource)
	{
		return get_log(asource, ELOG_WARN, ELOG_DEFAULT, false);
	}

	ptr_logitem log_warn_net(const std::source_location& asource)
	{
		return get_log(asource, ELOG_WARN, ELOG_DEFAULT, true);
	}

	ptr_logitem log_error(const std::source_location& asource)
	{
		return get_log(asource, ELOG_ERROR, ELOG_DEFAULT, false);
	}

	ptr_logitem log_error_net(const std::source_location& asource)
	{
		return get_log(asource, ELOG_ERROR, ELOG_DEFAULT, true);
	}

	ptr_logitem log_bi(const std::source_location& asource)
	{
		return get_log(asource, ELOG_ERROR, ELOG_BI, true);
	}
}// namespace ngl
