/*
* Copyright (c) [2020-2025] NingLeixueR
*
* Project Name: ngl_server
* Project URL: https://github.com/NingLeixueR/ngl_server
*
* This file is part of the ngl_server project, released under the MIT License.
* You are free to use, modify, and distribute this project in accordance with the license terms,
* including for commercial purposes, provided that you retain the original copyright and license notice.
*
* For full license details, see the LICENSE file in the project root directory:
* https://github.com/NingLeixueR/ngl_server/blob/main/LICENSE
*/
// File overview: Implements logic for xml.


#include "tools/tab/xml/xmlinfo.h"

#include <string_view>

namespace ngl
{
	namespace xmlinfo_detail
	{
		constexpr char ascii_tolower(char avalue)
		{
			return (avalue >= 'A' && avalue <= 'Z') ? static_cast<char>(avalue - 'A' + 'a') : avalue;
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
			const std::string_view trimmed = tools::trim_ascii_spaces(avalue);
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
	} // namespace xmlinfo_detail

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
		if (xmlinfo_detail::parse_text_bool(*lp, adata))
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

	bool xarg_info::find_view(const char* akey, std::string_view& adata) const
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
