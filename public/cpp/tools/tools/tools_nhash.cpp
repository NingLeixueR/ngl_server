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
// File overview: Implements logic for tools.


#include "tools/tools/tools_nhash.h"

namespace ngl::tools
{
	std::map<int64_t, std::map<std::string, int32_t>> nhash::m_hash;
	std::shared_mutex nhash::m_mutex;
}//namespace ngl::tools
