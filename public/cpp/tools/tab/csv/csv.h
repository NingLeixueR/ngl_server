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

#include "tools/tools.h"

#include <functional>
#include <iostream>
#include <cstdint>
#include <fstream>
#include <string>
#include <tuple>
#include <vector>
#include <list>
#include <map>
#include <set>

namespace ngl
{
	struct csvpair
	{
		std::string m_data;
		int m_pos = 0;
		bool m_doublequotationmarks = false; // 双引号
		char m_fg = ',';//默认','

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
			std::string lvalue;
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
				lvalue += lret[lpos];
			}
			if (apair.m_doublequotationmarks)
			{
				return false;
			}
			adata = std::move(lvalue);
			return true;
		}
	};

	template <typename T>
	struct csv_read<std::vector<T>>
	{
		static bool read(csvpair& apair, std::vector<T>& adata)
		{
			std::string ltempstr;
			if (!csv_read<std::string>::read(apair, ltempstr))
			{
				return false;
			}
			csvpair lpair;
			lpair.m_data = ltempstr;
			lpair.m_fg = '*';
			std::vector<T> lparsed;
			for (; lpair.m_pos < lpair.m_data.size();)
			{
				T ltemp;
				if (!csv_read<T>::read(lpair, ltemp))
				{
					return false;
				}
				lparsed.push_back(std::move(ltemp));
			}
			adata = std::move(lparsed);
			return true;
		}
	};

	template <typename T>
	struct csv_read<std::list<T>>
	{
		static bool read(csvpair& apair, std::list<T>& adata)
		{
			std::string ltempstr;
			if (!csv_read<std::string>::read(apair, ltempstr))
			{
				return false;
			}
			csvpair lpair;
			lpair.m_data = ltempstr;
			lpair.m_fg = '*';
			std::list<T> lparsed;
			for (; lpair.m_pos < lpair.m_data.size();)
			{
				T ltemp;
				if (!csv_read<T>::read(lpair, ltemp))
				{
					return false;
				}
				lparsed.push_back(std::move(ltemp));
			}
			adata = std::move(lparsed);
			return true;
		}
	};

	template <typename T>
	struct csv_read<std::set<T>>
	{
		static bool read(csvpair& apair, std::set<T>& adata)
		{
			std::string ltempstr;
			if (!csv_read<std::string>::read(apair, ltempstr))
			{
				return false;
			}
			csvpair lpair;
			lpair.m_data = ltempstr;
			lpair.m_fg = '*';
			std::set<T> lparsed;
			for (; lpair.m_pos < lpair.m_data.size();)
			{
				T ltemp;
				if (!csv_read<T>::read(lpair, ltemp))
				{
					return false;
				}
				if (!lparsed.insert(std::move(ltemp)).second)
				{
					return false;
				}
			}
			adata = std::move(lparsed);
			return true;
		}
	};

	class rcsv
	{
	private:
		std::string m_data;
		int m_pos;

		template <typename TTUPLE, std::size_t... INDEX>
		static bool readcsv_impl(csvpair& apair, std::index_sequence<INDEX...>, TTUPLE& avalues)
		{
			return (csv_read<std::tuple_element_t<INDEX, TTUPLE>>::read(apair, std::get<INDEX>(avalues)) && ...);
		}

		template <typename TTUPLE, std::size_t... INDEX, typename ...ARG>
		static void assigncsv_impl(std::index_sequence<INDEX...>, TTUPLE& avalues, ARG&... args)
		{
			((args = std::move(std::get<INDEX>(avalues))), ...);
		}
	public:
		bool read(const std::string& aname, std::string& averify);

		rcsv() :
			m_pos(0)
		{
		}

		template <typename ...ARG>
		static bool readcsv(csvpair& apair, ARG& ...args)
		{
			if (apair.m_data.empty())
			{
				return false;
			}
			std::tuple<std::decay_t<ARG>...> lparsed;
			if (!readcsv_impl(apair, std::make_index_sequence<sizeof...(ARG)>{}, lparsed))
			{
				return false;
			}
			assigncsv_impl(std::make_index_sequence<sizeof...(ARG)>{}, lparsed, args...);
			return true;
		}

		template <typename T>
		bool readcsv(std::map<int, T>& adata)
		{
			std::map<int, T> lparsed;
			for (;;)
			{
				csvpair lpair;
				if (!csv_helper::readline(m_data, m_pos, lpair.m_data))
				{
					break;
				}
				if (!lpair.m_data.empty())
				{
					if (lpair.m_data[0] == '#')
					{
						continue;
					}
					T ltemp;
					if (!ltemp.rcsv(lpair))
					{
						return false;
					}
					if (ltemp.m_id != 0)
					{
						if (!lparsed.emplace(ltemp.m_id, std::move(ltemp)).second)
						{
							return false;
						}
					}
				}
			}
			adata = std::move(lparsed);
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
		if (!csv_read<std::string>::read(apair, ltemp))
		{
			return false;
		}
		TNUMBER lvalue = TNUMBER();
		if (!ltemp.empty())
		{
			try
			{
				lvalue = tools::lexical_cast<TNUMBER>(ltemp.c_str());
			}
			catch (...)
			{
				return false;
			}
		}
		adata = lvalue;
		return true;
	}
}//namespace ngl
