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
// File overview: Implements misc helpers for tools.

#include "tools/tools/tools_misc.h"

#include <memory>

#if defined(__GNUC__) || defined(__clang__)
#include <cxxabi.h>
#endif

namespace ngl::tools
{
	namespace name_detail
	{
		void erase_parts(std::string& avalue, std::string_view aneedle)
		{
			if (aneedle.empty())
			{
				return;
			}

			std::size_t lpos = avalue.find(aneedle);
			while (lpos != std::string::npos)
			{
				avalue.erase(lpos, aneedle.size());
				lpos = avalue.find(aneedle, lpos);
			}
		}

		void erase_char(std::string& avalue, char achar)
		{
			avalue.erase(std::remove(avalue.begin(), avalue.end(), achar), avalue.end());
		}
	}

	std::string& type_name_handle(std::string& aname)
	{
#if defined(__GNUC__) || defined(__clang__)
		int lstat = 0;
		std::unique_ptr<char, void(*)(char*)> ldem(
			abi::__cxa_demangle(aname.c_str(), nullptr, nullptr, &lstat),
			[](char* aptr)
			{
				std::free(aptr);
			});
		if (lstat == 0 && ldem != nullptr)
		{
			aname = ldem.get();
		}
#endif
		name_detail::erase_parts(aname, "struct ");
		name_detail::erase_parts(aname, "class ");
		name_detail::erase_parts(aname, "ngl::");
		name_detail::erase_parts(aname, "pbdb::");
		name_detail::erase_parts(aname, "pbnet::");
		name_detail::erase_parts(aname, "pbexample::");
		name_detail::erase_char(aname, ' ');
		return aname;
	}

	std::vector<const char*> split_str(char* apbuff, int32_t abuff_cnt)
	{
		std::vector<const char*> lbufs;
		if (apbuff == nullptr || abuff_cnt <= 0)
		{
			return lbufs;
		}
		lbufs.reserve(static_cast<std::size_t>(std::count(apbuff, apbuff + abuff_cnt, ',')) + 1);
		int32_t lbeg = 0;
		for (int32_t lidx = 0; lidx < abuff_cnt; ++lidx)
		{
			if (apbuff[lidx] == ',')
			{
				apbuff[lidx] = '\0';
				lbufs.emplace_back(&apbuff[lbeg]);
				lbeg = lidx + 1;
				if (lbeg < abuff_cnt && apbuff[lbeg] == ' ')
				{
					++lbeg;
				}
			}
		}
		lbufs.emplace_back(&apbuff[lbeg]);
		return lbufs;
	}

	bool bit(int32_t atype, int32_t acjson)
	{
		return (atype & acjson) != 0;
	}

	int32_t less_member()
	{
		return 0;
	}

	void erase_repeat(std::string& astr, char ach)
	{
		auto liter = std::unique(astr.begin(), astr.end(), [ach](char al, char ar)
			{
				return al == ach && ar == ach;
			});
		astr.erase(liter, astr.end());
	}

	std::string_view trim_ascii_spaces(std::string_view avalue)
	{
		while (!avalue.empty() && std::isspace(static_cast<unsigned char>(avalue.front())) != 0)
		{
			avalue.remove_prefix(1);
		}
		while (!avalue.empty() && std::isspace(static_cast<unsigned char>(avalue.back())) != 0)
		{
			avalue.remove_suffix(1);
		}
		return avalue;
	}

	std::size_t strlen(const char* astr)
	{
		return astr == nullptr ? 0 : std::string_view(astr).size();
	}
}//namespace ngl::tools
