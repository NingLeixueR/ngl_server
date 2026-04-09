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
// File overview: Declares hashing helpers for tools.

#pragma once

#include "tools/tools/tools_core.h"

namespace ngl::tools
{
	std::string md5(const std::string& atext);
	std::string sh1(std::string_view atext);
	std::string hmac_sha1(const std::string& akey, const std::string& atext);
}//namespace ngl::tools
