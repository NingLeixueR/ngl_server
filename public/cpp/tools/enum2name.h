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
// File overview: Declares interfaces for tools.

#pragma once

#include "tools/tools.h"

#include <algorithm>
#include <string>
#include <map>

namespace ngl
{
	// Bidirectional enum <-> name registry with original/lower/upper variants.
	template <typename ENUMT>
	class enum2name
	{
		enum2name() = delete;
		enum2name(const enum2name&) = delete;
		enum2name& operator=(const enum2name&) = delete;

		struct data
		{
			std::string m_tolower;		// Lower-case lookup form.
			std::string m_toupper;		// Upper-case lookup form.
			std::string m_never;		// Original registration spelling.
		};
		static std::map<int, std::map<ENUMT, data>>			m_datae2n;
		static std::map<int, std::map<std::string, ENUMT>>	m_datan2e;

		static void rename(std::string& astr)
		{
			tools::transform_tolower(astr);
		}
	public:
		static bool empty()
		{
			return m_datae2n.empty();
		}

		static void set(ENUMT aenum, const char* aname, int anum = 0)
		{
			data& ltemp = m_datae2n[anum][aenum];
			ltemp.m_never = aname;
			ltemp.m_tolower = aname;
			ltemp.m_toupper = aname;
			tools::transform_tolower(ltemp.m_tolower);
			tools::transform_toupper(ltemp.m_toupper);
			m_datan2e[anum][ltemp.m_never] = aenum;
			m_datan2e[anum][ltemp.m_tolower] = aenum;
			m_datan2e[anum][ltemp.m_toupper] = aenum;
		}

		static ENUMT enum_null()
		{
			return (ENUMT)-1;
		}

		static ENUMT get_enum(const char* aname, int anum = 0)
		{
			std::string str(aname);
			rename(str);
			auto lpmap = tools::findmap(m_datan2e, anum);
			if (lpmap == nullptr)
			{
				return enum_null();
			}
			auto lpenum = tools::findmap(*lpmap, str);
			if (lpenum == nullptr)
			{
				return enum_null();
			}
			return *lpenum;
		}

		static const data* get_data(const ENUMT aenum, int anum = 0)
		{
			auto lpmap = tools::findmap(m_datae2n, anum);
			if (lpmap == nullptr)
			{
				return nullptr;
			}
			auto lpdata = tools::findmap(*lpmap, aenum);
			if (lpdata == nullptr)
			{
				return nullptr;
			}
			return lpdata;
		}

		static const char* name(const ENUMT aenum, int anum = 0)
		{
			const data* ltempdata = get_data(aenum, anum);
			if (ltempdata == nullptr)
			{
				return nullptr;
			}
			return ltempdata->m_never.c_str();
		}

		static const char* tolower_name(const ENUMT aenum, int anum = 0)
		{
			const data* ltempdata = get_data(aenum, anum);
			if (ltempdata == nullptr)
			{
				return nullptr;
			}
			return ltempdata->m_tolower.c_str();
		}

		static const char* toupper_name(const ENUMT aenum, int anum = 0)
		{
			const data* ltempdata = get_data(aenum, anum);
			if (ltempdata == nullptr)
			{
				return nullptr;
			}
			return ltempdata->m_toupper.c_str();
		}
		
		static void print()
		{
			for (auto& [_num, _map] : m_datae2n)
			{
				for (auto& [_enum, _data] : _map)
				{
					std::cout << std::format("{}:{}", (int)_enum, _data.m_never) << std::endl;
				}
			}
		}
	};

	template <typename ENUMT>
	std::map<int, std::map<ENUMT, typename enum2name<ENUMT>::data>> enum2name<ENUMT>::m_datae2n;

	template <typename ENUMT>
	std::map<int, std::map<std::string, ENUMT>> enum2name<ENUMT>::m_datan2e;

	template <typename ENUMT>
	using em = enum2name<ENUMT>;

	#define em_pram(NAME) NAME,#NAME
}// namespace ngl
