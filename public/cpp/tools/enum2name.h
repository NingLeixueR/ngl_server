#pragma once

#include "tools.h"

#include <algorithm>
#include <string>
#include <map>

namespace ngl
{
	// # 用于关联枚举和字符串名字
	template <typename ENUMT>
	class enum2name
	{
		enum2name() = delete;
		enum2name(const enum2name&) = delete;
		enum2name& operator=(const enum2name&) = delete;

		struct data
		{
			std::string m_tolower;		// 转换为小写
			std::string m_toupper;		// 转换为大写
			std::string m_never;		// 不改变大小写
		};
		static std::map<int, std::map<ENUMT, data>> m_datae2n;
		static std::map<int, std::map<std::string, ENUMT>> m_datan2e;

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
			auto itor1 = m_datan2e.find(anum);
			if (itor1 == m_datan2e.end())
			{
				return enum_null();
			}
			auto itor2 = itor1->second.find(str);
			if (itor2 == itor1->second.end())
			{
				return enum_null();
			}
			return itor2->second;
		}

		static const data* get_data(const ENUMT aenum, int anum = 0)
		{
			auto itor1 = m_datae2n.find(anum);
			if (itor1 == m_datae2n.end())
			{
				return nullptr;
			}
			auto itor2 = itor1->second.find(aenum);
			if (itor2 == itor1->second.end())
			{
				return nullptr;
			}
			return &itor2->second;
		}

		static const char* get_name(const ENUMT aenum, int anum = 0)
		{
			const data* ltempdata = get_data(aenum, anum);
			if (ltempdata == nullptr)
			{
				return nullptr;
			}
			return ltempdata->m_never.c_str();
		}

		static const char* get_tolower_name(const ENUMT aenum, int anum = 0)
		{
			const data* ltempdata = get_data(aenum, anum);
			if (ltempdata == nullptr)
			{
				return nullptr;
			}
			return ltempdata->m_tolower.c_str();
		}

		static const char* get_toupper_name(const ENUMT aenum, int anum = 0)
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
			for (const auto& item : m_datae2n)
			{
				for (const auto& ipair : item.second)
				{
					std::cout << std::format("{}:{}", (int)ipair.first, ipair.secod.m_never) << std::endl;
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