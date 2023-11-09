#include "operator_file.h"

#include <fstream>
#include <string>

namespace ngl
{
	readfile::readfile(const std::string& filename)
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
	void readfile::jumpbegin(int anum, bool aiscsv)
	{
		if (aiscsv == false)
		{
			std::string line;
			while (--anum >= 0)
			{
				getline(m_file, line);
			}
		}
		else
		{
			int32_t lisshuyin = 0;
			while (anum > 0)
			{
				std::string line;
				getline(m_file, line);
				for (int i = 0; i < line.size(); ++i)
				{
					if (line[i] == '\"')
						++lisshuyin;
				}
				if(lisshuyin%2 == 0)
					--anum;
			}
		}
	}

	bool readfile::readline(std::string& aline)
	{
		if (m_file.eof())
			return false;
		getline(m_file, aline);
		return true;
	}

	std::pair<char*, int> readfile::readcurrent()
	{
		if (m_file.is_open())
		{
			size_t lsizecurrent = m_file.tellg();
			m_file.seekg(0, std::ios::end);
			size_t lsize = m_file.tellg();
			m_file.seekg(lsizecurrent, std::ios::beg);
			std::pair<char*, int> lpair(new char[lsize - lsizecurrent + 1], lsize - lsizecurrent);
			memset(lpair.first, 0x0, lpair.second + 1);
			m_file.read(lpair.first, lpair.second);
			//aneirong.assign(lbuff.get());
			return lpair;
		}
		return std::pair<char*, int>(nullptr, 0);
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

	bool filetools::remove(const std::string& afilename)
	{
		return ::remove(afilename.c_str()) == 0;
	}


	bool filetools::exist(const std::string& afilename)
	{
		std::ifstream f(afilename.c_str());
		return f.good();
	}
}