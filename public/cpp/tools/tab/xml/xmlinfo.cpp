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
	namespace
	{
		constexpr char ascii_tolower(char avalue)
		{
			return (avalue >= 'A' && avalue <= 'Z') ? static_cast<char>(avalue - 'A' + 'a') : avalue;
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

		bool equals_ascii_icase(std::string_view alhs, const char* arhs)
		{
			std::size_t index = 0;
			for (; arhs[index] != '\0'; ++index)
			{
				if (index >= alhs.size() || ascii_tolower(alhs[index]) != arhs[index])
				{
					return false;
				}
			}
			if (index != alhs.size())
			{
				return false;
			}
			return true;
		}

		bool parse_text_bool(std::string_view avalue, bool& aout)
		{
			const std::string_view trimmed = trim_ascii_spaces(avalue);
			if (trimmed.empty())
			{
				return false;
			}
			switch (trimmed.size())
			{
			case 1:
				if (trimmed[0] == '1')
				{
					aout = true;
					return true;
				}
				if (trimmed[0] == '0')
				{
					aout = false;
					return true;
				}
				return false;
			case 2:
				if (equals_ascii_icase(trimmed, "on"))
				{
					aout = true;
					return true;
				}
				if (equals_ascii_icase(trimmed, "no"))
				{
					aout = false;
					return true;
				}
				return false;
			case 3:
				if (equals_ascii_icase(trimmed, "yes"))
				{
					aout = true;
					return true;
				}
				if (equals_ascii_icase(trimmed, "off"))
				{
					aout = false;
					return true;
				}
				return false;
			case 4:
				if (equals_ascii_icase(trimmed, "true"))
				{
					aout = true;
					return true;
				}
				return false;
			case 5:
				if (equals_ascii_icase(trimmed, "false"))
				{
					aout = false;
					return true;
				}
				return false;
			default:
				return false;
			}
		}
	}

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
		if (parse_text_bool(*lp, adata))
		{
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
