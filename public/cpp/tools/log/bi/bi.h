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

#include "tools.h"

#include <string>

namespace ngl
{
	///*class bi
	//{
	//	template <typename T>
	//	static void add(std::string& astr, const T& avalue)
	//	{
	//		if (!astr.empty())
	//		{
	//			astr += '|';
	//		}
	//		astr += tools::lexical_cast<std::string>(avalue);
	//	}

	//	template <typename T, typename ...ARGS>
	//	static void add(std::string& astr, const T& avalue, const ARGS&... args)
	//	{
	//		add(astr, avalue);
	//		add(astr, args...);
	//	}

	//public:
	//	template <typename T, typename ...ARGS>
	//	static std::string push(const T& avalue, const ARGS&... args)
	//	{
	//		std::string lstr;
	//		add(lstr, args...);
	//		return lstr;
	//	}
	//};*/

    class bi
    {
        template <typename T>
        static void add(std::string& astr, const T& value)
        {
            if (!astr.empty())
            {
				astr += '|'; // 非空时先加分隔符
            }
			astr += tools::lexical_cast<std::string>(value);
        }
    public:
        template <typename... Args>
        static std::string push(const Args&... args)
        {
			std::string result;
			(add(result, args), ...);
			return result;
        }
    };
}//namespace ngl