#pragma once

#include <fstream>
#include <string>
#include <memory>
#include <vector>

namespace ngl
{
	class readfile
	{
		std::ifstream m_file;
	public:
		readfile(const std::string& filename);
		~readfile();
		// 跳过前anum行
		void jumpbegin(int anum, bool aiscsv);
		bool readline(std::string& aline);
		bool readcurrent(std::string& astr);
		void read(std::string& aneirong);
	};

	class writefile
	{
		std::ofstream m_file;
	public:
		writefile(const std::string& filename);
		~writefile();
		void write(const std::string& aneirong);
	};

	class filetools
	{
	public:
		static bool exist(const std::string& afilename);
		static bool remove(const std::string& afilename);
		// 获取dir下所有目录
		static void dir(const std::string& apath, std::vector<std::string>& afilevec);
	};
}//namespace ngl