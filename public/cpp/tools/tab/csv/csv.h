/*
* Copyright (c) [2020-2025] NingLeixueR
* 
* 项目名称：ngl_server
* 项目地址：https://github.com/NingLeixueR/ngl_server
* 
* 本文件是 ngl_server 项目的一部分，遵循 MIT 开源协议发布。
* 您可以按照协议规定自由使用、修改和分发本项目，包括商业用途，
* 但需保留原始版权和许可声明。
* 
* 许可详情参见项目根目录下的 LICENSE 文件：
* https://github.com/NingLeixueR/ngl_server/blob/main/LICENSE
*/
#pragma once

#include "tools.h"

#include <functional>
#include <iostream>
#include <cstdint>
#include <fstream>
#include <string>
#include <vector>
#include <list>
#include <map>
#include <set>

namespace ngl
{
	struct csvpair
	{
		std::string m_data;
		int m_pos;
		bool m_doublequotationmarks; // 双引号
		char m_fg;//默认','

		csvpair() :
			m_pos(0),
			m_doublequotationmarks(false),
			m_fg(',')
		{}

		void clear()
		{
			m_data.clear();
			m_pos = 0;
			m_doublequotationmarks = false;
		}
	};
	
	template <typename T>
	struct csv_read
	{
		static bool read(csvpair& apair, T& adata);
	};

	struct csv_helper
	{
		template <typename TNUMBER>
		static bool number(csvpair& apair, TNUMBER& adata);

		static bool readline(std::string& adata, int32_t& apos, std::string& aline)
		{
			aline.clear();
			bool lbool = false;
			auto lsize = (int)adata.size();
			for (; apos < lsize; ++apos)
			{
				if (!lbool)
				{
					if (adata[apos] == '\r')
					{
						continue;
					}
					if (adata[apos] == '\n')
					{
						++apos;
						return true;
					}
				}
				if (adata[apos] == '\"')
				{
					lbool = lbool ? false : true;
				}
				aline += adata[apos];
			}
			return !aline.empty();
		}
	};

	template <>
	struct csv_read<int8_t>
	{
		static bool read(csvpair& apair, int8_t& adata)
		{
			return csv_helper::number(apair, adata);
		}
	};

	template <>
	struct csv_read<int16_t>
	{
		static bool read(csvpair& apair, int16_t& adata)
		{
			return csv_helper::number(apair, adata);
		}
	};

	template <>
	struct csv_read<int32_t>
	{
		static bool read(csvpair& apair, int32_t& adata)
		{
			return csv_helper::number(apair, adata);
		}
	};

	template <>
	struct csv_read<int64_t>
	{
		static bool read(csvpair& apair, int64_t& adata)
		{
			return csv_helper::number(apair, adata);
		}
	};

	template <>
	struct csv_read<uint8_t>
	{
		static bool read(csvpair& apair, uint8_t& adata)
		{
			return csv_helper::number(apair, adata);
		}
	};

	template <>
	struct csv_read<uint16_t>
	{
		static bool read(csvpair& apair, uint16_t& adata)
		{
			return csv_helper::number(apair, adata);
		}
	};

	template <>
	struct csv_read<uint32_t>
	{
		static bool read(csvpair& apair, uint32_t& adata)
		{
			return csv_helper::number(apair, adata);
		}
	};

	template <>
	struct csv_read<uint64_t>
	{
		static bool read(csvpair& apair, uint64_t& adata)
		{
			return csv_helper::number(apair, adata);
		}
	};

	template <>
	struct csv_read<float>
	{
		static bool read(csvpair& apair, float& adata)
		{
			return csv_helper::number(apair, adata);
		}
	};

	template <>
	struct csv_read<double>
	{
		static bool read(csvpair& apair, double& adata)
		{
			return csv_helper::number(apair, adata);
		}
	};

	template <>
	struct csv_read<bool>
	{
		static bool read(csvpair& apair, bool& adata)
		{
			return csv_helper::number(apair, adata);
		}
	};

	template <>
	struct csv_read<std::string>
	{
		static bool read(csvpair& apair, std::string& adata)
		{
			std::string& lret = apair.m_data;
			int& lpos = apair.m_pos;
			auto lsize = (int)lret.size();
			for (; lpos < lsize; ++lpos)
			{
				if (!apair.m_doublequotationmarks)
				{
					if (lret[lpos] == apair.m_fg || lret[lpos] == '\n')
					{
						++lpos;
						break;
					}
					if (lret[lpos] == '\r')
					{
						continue;
					}
				}
				if (lret[lpos] == '\"')
				{
					apair.m_doublequotationmarks = apair.m_doublequotationmarks ? false : true;
					continue;
				}
				adata += lret[lpos];
			}
			return true;
		}
	};

	template <typename T>
	struct csv_read<std::vector<T>>
	{
		static bool read(csvpair& apair, std::vector<T>& adata)
		{
			std::string ltempstr;
			csv_read<std::string>::read(apair, ltempstr);
			csvpair lpair;
			lpair.m_data = ltempstr;
			lpair.m_fg = '*';
			for (; lpair.m_pos < lpair.m_data.size();)
			{
				T ltemp;
				if (csv_read<T>::read(lpair, ltemp))
				{
					adata.push_back(ltemp);
				}
			}
			lpair.m_fg = ',';
			return true;
		}
	};

	template <typename T>
	struct csv_read<std::list<T>>
	{
		static bool read(csvpair& apair, std::list<T>& adata)
		{
			std::string ltempstr;
			read(apair, ltempstr);
			csvpair lpair;
			lpair.m_data = ltempstr;
			lpair.m_fg = '*';
			for (; lpair.m_pos < lpair.m_data.size();)
			{
				T ltemp;
				if (csv_read<T>::read(lpair, ltemp))
				{
					adata.insert(ltemp);
				}
			}
			lpair.m_fg = ',';
			return true;
		}
	};

	template <typename T>
	struct csv_read<std::set<T>>
	{
		static bool read(csvpair& apair, std::set<T>& adata)
		{
			std::string ltempstr;
			csv_read<std::string>::read(apair, ltempstr);
			csvpair lpair;
			lpair.m_data = ltempstr;
			lpair.m_fg = '*';
			for (; lpair.m_pos < lpair.m_data.size();)
			{
				T ltemp;
				if (csv_read<T>::read(lpair, ltemp))
				{
					adata.insert(ltemp);
				}
			}
			lpair.m_fg = ',';
			return true;
		}
	};

	class rcsv
	{
	public:
		std::string m_data;
		int m_pos;
	public:
		bool read(const std::string& aname, std::string& averify);

		rcsv() :
			m_pos(0)
		{
		}
	private:
	public:
		template <typename T>
		static bool readcsv(csvpair& apair, T& adata)
		{
			if (apair.m_data.empty())
			{
				return false;
			}
			return csv_read<T>::read(apair, adata);
		}

		template <typename T, typename ...ARG>
		static bool readcsv(csvpair& apair, T& adata, ARG& ...arg)
		{
			if (apair.m_data.empty())
			{
				return false;
			}
			return readcsv(apair, adata) && readcsv(apair, arg...);
		}

		template <typename T>
		bool readcsv(std::map<int, T>& adata)
		{
			csvpair lpair;
			for (; csv_helper::readline(m_data, m_pos, lpair.m_data);)
			{
				if (!lpair.m_data.empty())
				{
					if (lpair.m_data[0] == '#')
					{
						continue;
					}
					T ltemp;
					if (ltemp.rcsv(lpair) && ltemp.m_id != 0)
					{
						adata.insert(std::make_pair(ltemp.m_id, ltemp));
						lpair.clear();
					}
				}
			}
			return true;
		}
	};
		
	template <typename T>
	bool csv_read<T>::read(csvpair& apair, T& adata)
	{
		if constexpr (std::is_enum<T>::value)
		{
			int32_t lvalue = 0;
			if (csv_read<int32_t>::read(apair, lvalue))
			{
				adata = (T)lvalue;
				return true;
			}
		}
		else
		{
			return adata.rcsv(apair);
		}
		return false;
	}

	template <typename TNUMBER>
	bool csv_helper::number(csvpair& apair, TNUMBER& adata)
	{
		std::string ltemp;
		csv_read<std::string>::read(apair, ltemp);
		if (ltemp != "")
		{
			adata = tools::lexical_cast<TNUMBER>(ltemp.c_str());
		}
		else
		{
			adata = TNUMBER();
		}
		return true;
	}
}//namespace ngl