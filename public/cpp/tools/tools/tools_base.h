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
// File overview: Declares base, endian, and base64 helpers for tools.

#pragma once

#include "tools/tools_core.h"

namespace ngl
{
	namespace tools
	{
		bool is_lanip(const std::string& aip);

		constexpr bool islittle()
		{
			return std::endian::native == std::endian::little;
		}

		int16_t transformlittle(parm<int16_t>& avalues);
		uint16_t transformlittle(parm<uint16_t>& avalues);
		int32_t transformlittle(parm<int32_t>& avalues);
		uint32_t transformlittle(parm<uint32_t>& avalues);
		int64_t transformlittle(parm<int64_t>& avalues);
		uint64_t transformlittle(parm<uint64_t>& avalues);

		std::string base64_encode(const char* adata, std::size_t alen);
		std::string base64_encode(const std::string& astr);
		std::string base64_decode(const char* adata, std::size_t alen);
		std::string base64_decode(const std::string& adata);

		bool uuid_make(std::string& astr);
	}
}
