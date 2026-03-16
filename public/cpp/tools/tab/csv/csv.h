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
// File overview: Declares interfaces for csv.

#pragma once

#include "tools/tools.h"

#include <charconv>
#include <cctype>
#include <cstdint>
#include <fstream>
#include <functional>
#include <iostream>
#include <string>
#include <string_view>
#include <system_error>
#include <limits>
#include <list>
#include <map>
#include <set>
#include <tuple>
#include <type_traits>
#include <vector>

namespace ngl
{
	struct csvpair
	{
		std::string m_data;
		int m_pos = 0;
		bool m_doublequotationmarks = false; // Translated comment.
		char m_fg = ',';// Default','

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
		static bool parse_number_text(std::string_view atext, TNUMBER& adata)
		{
			atext = tools::trim_ascii_spaces(atext);
			if (atext.empty())
			{
				adata = TNUMBER();
				return true;
			}

			if constexpr (std::is_same_v<TNUMBER, bool>)
			{
				if (atext == "1" || atext == "true" || atext == "TRUE" || atext == "True"
					|| atext == "yes" || atext == "YES" || atext == "on" || atext == "ON")
				{
					adata = true;
					return true;
				}
				if (atext == "0" || atext == "false" || atext == "FALSE" || atext == "False"
					|| atext == "no" || atext == "NO" || atext == "off" || atext == "OFF")
				{
					adata = false;
					return true;
				}
				return false;
			}
			else if constexpr (std::is_integral_v<TNUMBER>)
			{
				if constexpr (std::is_signed_v<TNUMBER>)
				{
					long long parsed = 0;
					const auto [ptr, ec] = std::from_chars(atext.data(), atext.data() + atext.size(), parsed);
					if (ec != std::errc() || ptr != atext.data() + atext.size())
					{
						return false;
					}
					if (parsed < static_cast<long long>(std::numeric_limits<TNUMBER>::min())
						|| parsed > static_cast<long long>(std::numeric_limits<TNUMBER>::max()))
					{
						return false;
					}
					adata = static_cast<TNUMBER>(parsed);
					return true;
				}
				else
				{
					unsigned long long parsed = 0;
					const auto [ptr, ec] = std::from_chars(atext.data(), atext.data() + atext.size(), parsed);
					if (ec != std::errc() || ptr != atext.data() + atext.size()
						|| parsed > static_cast<unsigned long long>(std::numeric_limits<TNUMBER>::max()))
					{
						return false;
					}
					adata = static_cast<TNUMBER>(parsed);
					return true;
				}
			}
			else
			{
				try
				{
					adata = tools::lexical_cast<TNUMBER>(std::string(atext).c_str());
					return true;
				}
				catch (...)
				{
					return false;
				}
			}
		}

		template <typename TNUMBER>
		static bool number(csvpair& apair, TNUMBER& adata);

		static bool readline(std::string& adata, int32_t& apos, std::string& aline)
		{
			aline.clear();
			bool lbool = false;
			auto lsize = (int)adata.size();
			if (apos >= 0 && apos < lsize)
			{
				aline.reserve(static_cast<std::size_t>(lsize - apos));
			}
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
				aline.push_back(adata[apos]);
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
			const int lsize = static_cast<int>(lret.size());
			std::string lvalue;
			if (lpos >= 0 && lpos < lsize)
			{
				lvalue.reserve(static_cast<std::size_t>(lsize - lpos));
			}

			int chunk_begin = lpos;
			for (; lpos < lsize; ++lpos)
			{
				const char current = lret[lpos];
				if (current == '\"')
				{
					if (chunk_begin < lpos)
					{
						lvalue.append(lret, static_cast<std::size_t>(chunk_begin), static_cast<std::size_t>(lpos - chunk_begin));
					}
					apair.m_doublequotationmarks = !apair.m_doublequotationmarks;
					chunk_begin = lpos + 1;
					continue;
				}

				if (!apair.m_doublequotationmarks)
				{
					if (current == apair.m_fg || current == '\n')
					{
						if (chunk_begin < lpos)
						{
							lvalue.append(lret, static_cast<std::size_t>(chunk_begin), static_cast<std::size_t>(lpos - chunk_begin));
						}
						++lpos;
						chunk_begin = lpos;
						break;
					}
					if (current == '\r')
					{
						if (chunk_begin < lpos)
						{
							lvalue.append(lret, static_cast<std::size_t>(chunk_begin), static_cast<std::size_t>(lpos - chunk_begin));
						}
						chunk_begin = lpos + 1;
						continue;
					}
				}
			}
			if (chunk_begin < lpos)
			{
				lvalue.append(lret, static_cast<std::size_t>(chunk_begin), static_cast<std::size_t>(lpos - chunk_begin));
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
			const int lsize = static_cast<int>(lpair.m_data.size());
			for (; lpair.m_pos < lsize;)
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
			const int lsize = static_cast<int>(lpair.m_data.size());
			for (; lpair.m_pos < lsize;)
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
			const int lsize = static_cast<int>(lpair.m_data.size());
			for (; lpair.m_pos < lsize;)
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
			return false;
		}
		else if constexpr (std::is_arithmetic_v<T>)
		{
			return csv_helper::number(apair, adata);
		}
		else
		{
			return adata.rcsv(apair);
		}
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
		if (!parse_number_text(std::string_view(ltemp), lvalue))
		{
			return false;
		}
		adata = lvalue;
		return true;
	}
}//namespace ngl