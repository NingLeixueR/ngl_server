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
// File overview: Declares interfaces for tools.

#pragma once

#include <fstream>
#include <string>
#include <memory>
#include <vector>

namespace ngl
{
	class readfile
	{
		readfile() = delete;
		readfile(const readfile&) = delete;
		readfile& operator=(const readfile&) = delete;

		std::ifstream m_file;
		int m_cached_maxline = -1;
	public:
		explicit readfile(const std::string& filename);

		~readfile();

		// # Whethersuccessful file
		bool is_open()const;

		// # Getfile
		int get_maxline();

		// # Beforeanum
		void jumpbegin(int anum, bool aiscsv);

		// # Read
		bool readline(std::string& aline);

		// # Readcurrentposition after allcontent
		bool readcurrent(std::string& astr);

		// # Read file content
		void read(std::string& aneirong);
	};

	class writefile
	{
		writefile() = delete;
		writefile(const writefile&) = delete;
		writefile& operator=(const writefile&) = delete;

		std::ofstream m_file;
	public:
		writefile(const std::string& filename);

		~writefile();

		// # Tofilewritecontent
		void write(const std::string& aneirong);
	};
}//namespace ngl