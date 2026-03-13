/*
* Copyright (c) [2020-2025] NingLeixueR
*
* Project: ngl_server
* License: MIT
*/

#include "tools/tab/xml/xmlinfo.h"

#include <algorithm>
#include <cctype>
#include <string_view>

namespace ngl
{
	const std::string* xarg_info::find_raw(const char* akey) const
	{
		if (akey == nullptr)
		{
			return nullptr;
		}
		return tools::findmap(m_data, akey);
	}

	bool xarg_info::find(const char* akey, bool& adata) const
	{
		const std::string* lp = find_raw(akey);
		if (lp == nullptr)
		{
			return false;
		}

		std::string normalized;
		normalized.reserve(lp->size());
		for (unsigned char ch : *lp)
		{
			if (std::isspace(ch) == 0)
			{
				normalized.push_back(static_cast<char>(std::tolower(ch)));
			}
		}

		if (normalized == "true" || normalized == "1" || normalized == "yes" || normalized == "on")
		{
			adata = true;
			return true;
		}
		if (normalized == "false" || normalized == "0" || normalized == "no" || normalized == "off")
		{
			adata = false;
			return true;
		}

		try
		{
			adata = tools::lexical_cast<bool>(*lp);
			return true;
		}
		catch (...)
		{
			return false;
		}
	}

	bool xarg_info::find(const char* akey, std::string& adata) const
	{
		const std::string* lp = find_raw(akey);
		if (lp == nullptr)
		{
			return false;
		}
		adata = *lp;
		return true;
	}
}// namespace ngl
