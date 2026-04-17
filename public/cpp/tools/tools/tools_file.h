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
// File overview: File and directory I/O helpers for reading, writing, and path manipulation.

#pragma once

#include <fstream>
#include <string>
#include <memory>
#include <vector>

namespace ngl::tools
{
	class readfile
	{
		readfile() = delete;
		readfile(const readfile&) = delete;
		readfile& operator=(const readfile&) = delete;

		std::ifstream m_file;
		int m_cached_maxline = -1; // Lazily computed line count cache.
	public:
		explicit readfile(const std::string& filename);

		~readfile();

		// Query file state and support line-based or whole-file reads.
		bool is_open()const;

		int get_maxline();

		// Skip the first anum logical lines; CSV mode respects quoted embedded newlines.
		void jumpbegin(int anum, bool aiscsv);

		bool readline(std::string& aline);

		// Append from the current read cursor to EOF.
		bool readcurrent(std::string& astr);

		// Read the full file from the beginning.
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

		// Write raw bytes to the file.
		void write(const std::string& aneirong);
	};
}//namespace ngl
