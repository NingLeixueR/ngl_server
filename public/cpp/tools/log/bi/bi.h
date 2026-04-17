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
// File overview: Business intelligence event collector for analytics and telemetry.

#pragma once

#include "tools/tools.h"

#include <string>

namespace ngl
{
    class bi
    {
        template <typename T>
        static void add(std::string& astr, const T& value)
        {
            if (!astr.empty())
            {
				astr += '|';
            }
			astr += tools::lexical_cast<std::string>(value);
        }
    public:
        template <typename... Args>
        static std::string push(const Args&... args)
        {
			std::string result;
			return (add(result, args), ... , result);
        }
    };
}//namespace ngl