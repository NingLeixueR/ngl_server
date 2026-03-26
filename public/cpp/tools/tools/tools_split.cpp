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
// File overview: Implements split and splice helpers for tools.

#include "tools_split.h"

namespace ngl
{
	namespace tools
	{
		bool splite(const char* abuff, const char* afg, std::vector<std::string>& avec)
		{
			if (abuff == nullptr || afg == nullptr || *afg == '\0')
			{
				return false;
			}
			const std::size_t lfg_len = tools::strlen(afg);
			const std::string_view lsrc(abuff);
			const std::string_view lfg(afg, lfg_len);
			std::size_t lbeg = 0;
			while (true)
			{
				const std::size_t lpos = lsrc.find(lfg, lbeg);
				if (lpos == std::string_view::npos)
				{
					break;
				}
				avec.emplace_back(lsrc.substr(lbeg, lpos - lbeg));
				lbeg = lpos + lfg_len;
			}
			if (lbeg < lsrc.size())
			{
				avec.emplace_back(lsrc.substr(lbeg));
			}
			return !avec.empty();
		}

		bool splicing(const std::vector<std::string>& avec, const char* afg, std::string& astr, int32_t apos)
		{
			if (afg == nullptr || apos < 0)
			{
				return false;
			}
			const std::size_t lpos = static_cast<std::size_t>(apos);
			if (lpos > avec.size())
			{
				return false;
			}

			const std::size_t lfg_len = tools::strlen(afg);
			for (std::size_t li = lpos; li < avec.size(); ++li)
			{
				if (li != lpos)
				{
					astr.append(afg, lfg_len);
				}
				astr += avec[li];
			}
			return true;
		}

		bool splicing(const std::set<std::string>& aset, const char* afg, std::string& astr)
		{
			if (afg == nullptr)
			{
				return false;
			}
			const std::size_t lfg_len = tools::strlen(afg);
			std::size_t lapp_sz = aset.empty() ? 0 : lfg_len * (aset.size() - 1);
			for (const auto& litem : aset)
			{
				lapp_sz += litem.size();
			}
			astr.reserve(astr.size() + lapp_sz);

			for (auto liter = aset.begin(); liter != aset.end(); ++liter)
			{
				if (liter != aset.begin())
				{
					astr.append(afg, lfg_len);
				}
				astr += *liter;
			}
			return true;
		}
	}
}
