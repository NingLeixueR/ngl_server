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
	public:
		explicit readfile(const std::string& filename);
		~readfile();
		// # 是否成功打开文件
		bool is_open()const;
		// # 获取文件的行数
		int get_maxline();
		// # 跳过前anum行
		void jumpbegin(int anum, bool aiscsv);
		bool readline(std::string& aline);
		bool readcurrent(std::string& astr);
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
		void write(const std::string& aneirong);
	};
}//namespace ngl