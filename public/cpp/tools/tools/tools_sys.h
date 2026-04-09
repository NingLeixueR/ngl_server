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
// File overview: Declares system and filesystem helpers for tools.

#pragma once

#include "tools_core.h"

namespace ngl::tools
{
	std::string time2str(int autc, const char* aformat = "%y/%m/%d %H:%M:%S");

	const std::string& server_name();

	int rand();

	bool directories_exists(const std::string& apath);
	bool file_exists(const std::string& apath);
	bool create_dir(const std::string& apath);
	bool file_remove(const std::string& afile);
	void dir(const std::string& apath, std::vector<std::string>& afiles, bool aiter = false);

	void no_core_dump(bool anocreate = false);

	std::function<void()> send_mail(const std::string& acontent);

	int64_t nguidstr2int64(const char* anguid);
}//namespace ngl::tools
