#pragma once

#include <fstream>
#include <string>
#include <memory>

namespace ngl
{
	class readfile
	{
		std::ifstream m_file;
	public:
		readfile(const std::string& filename);
		~readfile();
		// Ìø¹ýÇ°anumÐÐ
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
	};
}//namespace ngl