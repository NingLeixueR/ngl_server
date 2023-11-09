#include "operator_file.h"

#include <fstream>
#include <string>

namespace ngl
{
	readfile::readfile(const std::string filename)
	{
		m_file.open(filename, std::ios::binary);
	}
	readfile::~readfile()
	{
		if (m_file.is_open())
		{
			m_file.close();
		}
	}
	//跳过前三行
	void readfile::jumpbegin(int anum)
	{
		std::string line;
		while (--anum >= 0)
		{
			//getline(m_file, line);
		}
	}

	void readfile::readcurrent(std::string& aneirong)
	{
		if (m_file.is_open())
		{
			size_t lsizecurrent = m_file.tellg();
			m_file.seekg(0, std::ios::end);
			size_t lsize = m_file.tellg();
			m_file.seekg(lsizecurrent, std::ios::beg);
			char* lp = new char[lsize - lsizecurrent + 1];
			memset(lp, 0x0, lsize - lsizecurrent + 1);
			std::unique_ptr<char[]> lbuff(lp);
			m_file.read(lbuff.get(), lsize - lsizecurrent);
			aneirong.assign(lbuff.get());
		}
	}

	void readfile::read(std::string& aneirong)
	{
		if (m_file.is_open())
		{
			m_file.seekg(0, std::ios::end);
			size_t lsize = m_file.tellg();
			m_file.seekg(0, std::ios::beg);
			char* lp = new char[lsize + 1];
			memset(lp, 0x0, lsize + 1);
			std::unique_ptr<char[]> lbuff(lp);
			m_file.read(lbuff.get(), lsize);
			aneirong.assign(lbuff.get());
		}
	}


	writefile::writefile(const std::string& filename)
	{
		m_file.open(filename, std::ios::binary);
	}

	void writefile::write(const std::string& aneirong)
	{
		if (m_file.is_open())
		{
			m_file.write(aneirong.c_str(), aneirong.size());
		}
	}

	writefile::~writefile()
	{
		if (m_file.is_open())
		{
			m_file.close();
		}
	}

}