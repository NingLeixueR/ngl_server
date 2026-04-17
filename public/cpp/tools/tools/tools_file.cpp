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
// File overview: Implements file and directory I/O operations.


#include "actor/protocol/nprotocol.h"
#include "tools/tools/tools_file.h"
#include "tools/log/nlog.h"

#include <filesystem>
#include <fstream>
#include <limits>
#include <string>

namespace ngl::tools
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
		if (!m_file.is_open())
		{
			return 0;
		}
		if (m_cached_maxline >= 0)
		{
			return m_cached_maxline;
		}

		const std::streampos lcurrent = m_file.tellg();
		m_file.clear();
		m_file.seekg(0, std::ios::beg);

		std::string line;
		int32_t lline = 0;
		while (std::getline(m_file, line))
		{
			++lline;
		}

		m_file.clear();
		if (lcurrent != std::streampos(-1))
		{
			// Restore the caller's original cursor so get_maxline() stays side-effect free.
			m_file.seekg(lcurrent);
		}
		else
		{
			m_file.seekg(0, std::ios::beg);
		}
		m_cached_maxline = lline;
		return m_cached_maxline;
	}

	// Before
	void readfile::jumpbegin(int anum, bool aiscsv)
	{
		std::string line;
		if (aiscsv == false)
		{
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
			bool linside_quote = false;
			while (anum > 0)
			{
				if (std::getline(m_file, line))
				{
					// CSV logical rows can span multiple physical lines while inside quotes.
					for (std::size_t i = 0; i < line.size(); ++i)
					{
						if (line[i] == '\"')
						{
							linside_quote = !linside_quote;
						}
					}
					if (!linside_quote)
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
			m_file.clear();
			const std::streamoff lsizecurrent = m_file.tellg();
			if (lsizecurrent < 0)
			{
				return false;
			}
			m_file.seekg(0, std::ios::end);
			const std::streamoff lsize = m_file.tellg();
			m_file.seekg(lsizecurrent, std::ios::beg);

			if (lsize <= lsizecurrent)
			{
				return false;
			}	
			const size_t lstrlen = astr.size();
			const size_t lappend = static_cast<size_t>(lsize - lsizecurrent);
			astr.resize(lstrlen + lappend);
			m_file.read(&astr.data()[lstrlen], static_cast<std::streamsize>(lappend));
			return true;
		}
		return false;
	}

	void readfile::read(std::string& aneirong)
	{
		if (m_file.is_open())
		{
			m_file.clear();
			m_file.seekg(0, std::ios::end);
			const std::streamoff lsize = m_file.tellg();
			if (lsize < 0)
			{
				aneirong.clear();
				return;
			}
			m_file.seekg(0, std::ios::beg);
			if (lsize == 0) 
			{
				aneirong.clear();
				return;
			}
			aneirong.resize(static_cast<size_t>(lsize));
			m_file.read(aneirong.data(), static_cast<std::streamsize>(lsize));
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
			const std::size_t lsize = aneirong.size();
			if (lsize > static_cast<std::size_t>(std::numeric_limits<std::streamsize>::max()))
			{
				log_error()->print("writefile::write content too large [{}]", lsize);
				return;
			}
			m_file.write(aneirong.c_str(), static_cast<std::streamsize>(lsize));
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
