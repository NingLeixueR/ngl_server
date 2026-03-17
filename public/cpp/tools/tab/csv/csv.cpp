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
// File overview: Implements logic for csv.


#include "tools/operator_file.h"
#include "tools/tab/csv/csv.h"
#include "tools/tools.h"

namespace ngl
{
	bool rcsv::read(const std::string& aname, std::string& averify)
	{
		m_pos = 0;
		m_data.clear();
		readfile lrf(aname);
		// Skip an optional UTF-8 BOM before parsing the first header row.
		lrf.jumpbegin(3, true);

		if (lrf.readcurrent(m_data) == false)
		{
			std::cout << std::format("loadcsv fail #{}", aname) << std::endl;
			return false;
		}
		averify = tools::md5(m_data);
		std::cout << std::format("loadcsv#{}#{}", aname, averify) << std::endl;
		return true;
	}
}// namespace ngl
