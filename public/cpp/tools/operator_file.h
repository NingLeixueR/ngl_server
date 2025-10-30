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

		// # 行读取
		bool readline(std::string& aline);

		// # 读取当前位置之后的全部内容
		bool readcurrent(std::string& astr);

		// # 读取整个文件的内容
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

		// # 向文件写入内容
		void write(const std::string& aneirong);
	};
}//namespace ngl