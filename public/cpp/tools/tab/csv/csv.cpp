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
// File overview: Implements CSV file parsing and row-to-struct mapping.


#include "actor/protocol/nprotocol.h"
#include "tools/tools/tools_file.h"
#include "tools/tab/csv/csv.h"
#include "tools/log/nlog.h"
#include "tools/tools.h"

namespace ngl
{
	bool rcsv::read(const std::string& aname, std::string& averify)
	{
		m_pos = 0;
		m_data.clear();
		tools::readfile lrf(aname);
		// Skip an optional UTF-8 BOM before parsing the first header row.
		lrf.jumpbegin(3, true);

		if (lrf.readcurrent(m_data) == false)
		{
			log_error()->print("loadcsv fail [{}]", aname);
			return false;
		}
		averify = tools::md5(m_data);
		log_info()->print("loadcsv [{}] verify={}", aname, averify);
		return true;
	}
}// namespace ngl
