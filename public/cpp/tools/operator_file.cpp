#include <filesystem>
#include <fstream>
#include <string>

#include "operator_file.h"
#include "nprotocol.h"
#include "nlog.h"

namespace ngl
{
	readfile::readfile(const std::string& filename)
	{
		m_file.open(filename, std::ios::binary);
	}

	readfile::~readfile()
	{
		try
		{
			if (m_file.is_open())
			{
				m_file.close();
			}
		}
		catch (...) 
		{
		}
	}

	bool readfile::is_open()const
	{
		return m_file.is_open();
	}

	int readfile::get_maxline()
	{
		std::string line;
		int32_t lline = 0;
		while (std::getline(m_file, line))
		{
			++lline;
		}
		return lline;
	}

	//跳过前三行
	void readfile::jumpbegin(int anum, bool aiscsv)
	{
		if (aiscsv == false)
		{
			std::string line;
			while (--anum >= 0)
			{
				if (!std::getline(m_file, line))
				{
					break;
				}
			}
		}
		else
		{
			int32_t lisshuyin = 0;
			while (anum > 0)
			{
				std::string line;
				if (std::getline(m_file, line))
				{
					int lsize = (int)line.size();
					for (int i = 0; i < lsize; ++i)
					{
						if (line[i] == '\"')
						{
							++lisshuyin;
						}
					}
					if (lisshuyin % 2 == 0)
					{
						--anum;
					}
				}
				else
				{
					break;
				}
			}
		}
	}

	bool readfile::readline(std::string& aline)
	{
		if (m_file.eof())
		{
			return false;
		}
		if (!std::getline(m_file, aline))
		{
			return false;
		}
		if (aline.empty() == false)
		{
			if (*aline.rbegin() == '\r')
			{
				aline.resize(aline.size() - 1);
			}
		}
		return true;
	}

	bool readfile::readcurrent(std::string& astr)
	{
		if (m_file.is_open())
		{
			int lsizecurrent = (int)m_file.tellg();
			if (lsizecurrent == -1)
			{
				return false;
			}
			m_file.seekg(0, std::ios::end);
			int lsize = (int)m_file.tellg();
			m_file.seekg(lsizecurrent, std::ios::beg);

			if (lsize == lsizecurrent)
			{
				return false;
			}	
			int lstrlen = (int)astr.size();
			astr.resize(lstrlen + (lsize - lsizecurrent));
			m_file.read(&astr.data()[lstrlen], lsize - lsizecurrent);
			return true;
		}
		return false;
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
		Try
		{
			if (m_file.is_open())
			{
				m_file.close();
			}
		}Catch
	}	
}// namespace ngl