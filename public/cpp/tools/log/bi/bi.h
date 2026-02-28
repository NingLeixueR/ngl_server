/*
* Copyright (c) [2020-2025] NingLeixueR
*
* 项目名称：ngl_server
* 项目地址：https://github.com/NingLeixueR/ngl_server
*
* 本文件是 ngl_server 项目的一部分，遵循 MIT 开源协议发布。
* 您可以按照协议规定自由使用、修改和分发本项目，包括商业用途，
* 但需保留原始版权和许可声明。
*
* 许可详情参见项目根目录下的 LICENSE 文件：
* https://github.com/NingLeixueR/ngl_server/blob/main/LICENSE
*/
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