#pragma once

#include "tools.h"

#include <algorithm>
#include <string>
#include <map>

namespace ngl
{
	// 用于关联枚举和字符串名字
	// ENUMT需要关联的枚举
	// TOLOWER是否统一将字符串进行小写处理

	enum enum_e2n
	{
		e2n_tolower,	// 转换为小写
		e2n_toupper,	// 转换为大写
		e2n_never,      // 不改变大小写
	};

	template <typename ENUMT, enum_e2n TOLOWER = e2n_never>
	class enum2name
	{
		static std::map<int, std::map<ENUMT, std::string>> m_datae2n;
		static std::map<int, std::map<std::string, ENUMT>> m_datan2e;

		static void change_name(std::string& astr)
		{
			if constexpr (TOLOWER == e2n_tolower)
			{
				std::transform(astr.begin(), astr.end(), astr.begin(), tolower);
			}
			if constexpr (TOLOWER == e2n_toupper)
			{
				std::transform(astr.begin(), astr.end(), astr.begin(), toupper);
			}			
		}
	public:
		static void set(ENUMT aenum, const char* aname, int anum = 0)
		{
			std::string str(aname);
			change_name(str);
			m_datae2n[anum][aenum] = str;
			m_datan2e[anum][str] = aenum;
		}

		static std::pair<ENUMT, bool> get_enum(const char* aname, int anum = 0)
		{
			std::string str(aname);
			change_name(str);
			auto itor1 = m_datan2e.find(anum);
			if(itor1 == m_datan2e.end())
				return std::make_pair((ENUMT)-1, false);
			auto itor2 = itor1->second.find(str);
			if (itor2 == itor1->second.end())
				return std::make_pair((ENUMT)-1, false);
			return std::make_pair(itor2->second, true);
		}

		static std::pair<const char*, bool> get_name(const ENUMT& aenum, int anum = 0)
		{
			auto itor1 = m_datae2n.find(anum);
			if (itor1 == m_datae2n.end())
				return std::make_pair("", false);
			auto itor2 = itor1->second.find(aenum);
			if (itor2 == itor1->second.end())
				return std::make_pair("", false);
			return std::make_pair(itor2->second.c_str(), true);
		}

		static const char* name(const ENUMT& aenum, int anum = 0)
		{
			auto itor1 = m_datae2n.find(anum);
			if (itor1 == m_datae2n.end())
				return nullptr;
			auto itor2 = itor1->second.find(aenum);
			if (itor2 == itor1->second.end())
				return nullptr;
			return itor2->second.c_str();
		}
	};

	template <typename ENUMT, enum_e2n TOLOWER/* = e2n_never*/>
	std::map<int, std::map<ENUMT, std::string>> enum2name<ENUMT, TOLOWER>::m_datae2n;

	template <typename ENUMT, enum_e2n TOLOWER/* = e2n_never*/>
	std::map<int, std::map<std::string, ENUMT>> enum2name<ENUMT, TOLOWER>::m_datan2e;

	template <typename ENUMT, enum_e2n TOLOWER = e2n_never>
	using em = enum2name<ENUMT, TOLOWER>;

	#define em_pram(NAME) NAME,#NAME
}// namespace ngl