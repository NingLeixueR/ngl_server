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
// File overview: Implements shared non-template helpers for tools.

#include "tools_core.h"
#include "tools/log/nlog.h"

namespace ngl
{
	namespace tools
	{
		void log_lex_err(
			const char* atotype,
			const char* afromtype,
			const char* aerror,
			const std::source_location& asource)
		{
			log_error(asource)->print("tools::lexical_cast<{}> failed from <{}> : {}", atotype, afromtype, aerror);
		}
	}
}
