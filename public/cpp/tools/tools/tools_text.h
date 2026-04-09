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
// File overview: Declares text, regex, url, and casing helpers for tools.

#pragma once

#include "tools_core.h"

namespace ngl
{
	namespace tools
	{
		bool wasscii2asscii(const std::wstring& awstr, std::string& astr);
		bool asscii2wasscii(const std::string& astr, std::wstring& awstr);
		bool wasscii2utf8(const std::wstring& awstr, std::string& astr);
		bool utf82wasscii(const std::string& astr, std::wstring& awstr);

		bool to_asscii(const std::string& astr1, std::string& astr2);
		bool to_utf8(const std::string& astr1, std::string& astr2);

		int32_t utf8firstbyte(uint8_t afirst);
		bool isutf8(const std::string& astr);
		bool isincludeutf8mb4(const std::string& astr);

		void sregex(const std::string& apattern, const std::string& adata, const std::function<void(std::string&)>& afun);
		void smatch(const std::string& aexpr, const std::string& adata, const std::function<void(std::smatch&)>& afun);
		void replace(const std::string& aexpr, const std::string& arepl, const std::string& adata, std::string& aret);
		void replace_ret(const std::string& aexpr, const std::string& arepl, std::string& adata, std::string& aret);

		std::string char2hex(char adec);
		std::string url_encode(const std::string& astr);
		std::string url_decode(const std::string& astr);

		std::vector<std::string_view> get_line(const char* apbuff, std::size_t abuff_sz);
		bool isnumber(const std::string& anum);
		void transform_tolower(std::string& adata);
		void transform_toupper(std::string& adata);
	}//namespace tools
}//namespace ngl
