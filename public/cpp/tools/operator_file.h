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

	class filetools
	{
		filetools() = delete;
		filetools(const filetools&) = delete;
		filetools& operator=(const filetools&) = delete;
	public:
		// # 是否存在某个文件
		static bool exist(const std::string& afilename);

		// # 移除某个文件
		static bool remove(const std::string& afilename);

		// # 获取dir下所有文件
		// # aiteration是否循环迭代dir下的所有目录
		static void dir(const std::string& apath, std::vector<std::string>& afilevec, bool aiteration = false);
	};
}//namespace ngl