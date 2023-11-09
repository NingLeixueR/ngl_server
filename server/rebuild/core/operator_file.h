#pragma once

#include <string>
#include <fstream>

namespace ngl
{
	class readfile
	{
		std::ifstream m_file;
	public:
		readfile(const std::string filename);
		~readfile();
		//跳过前三行
		void jumpbegin(int anum);
		void readcurrent(std::string& aneirong);
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
}