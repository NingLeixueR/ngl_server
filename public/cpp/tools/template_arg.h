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

#include "ndefine.h"
#include "tools.h"
#include "type.h"
#include <iostream>

namespace ngl
{
	/*
		辅助缺省参数: template_arg<TF,TARG...>
		TF需要实现static func<T>(TARG...)
	*/
	template <typename TF, typename ...TARG>
	class template_arg
	{
		template_arg() = delete;
		template_arg(const template_arg&) = delete;
		template_arg& operator=(const template_arg&) = delete;

	public:
		template <typename ...ARG>
		static void func(TARG... args)
		{
			(TF::template func<ARG>(args...), ...);
		}
	};

	template <typename TF>
	class template_arg_event
	{
		template_arg_event() = delete;
		template_arg_event(const template_arg_event&) = delete;
		template_arg_event& operator=(const template_arg_event&) = delete;

		template <typename TE>
		static void func2(i64_actorid aactorid, TE atype)
		{
			TF::func(aactorid, atype);
		}
	public:
		static void func()
		{
		}

		template <typename TE>
		static void func(i64_actorid aactorid, TE atype)
		{
			func2(aactorid, atype);
		}

		template <typename TE, typename ...ARG>
		static void func(i64_actorid aactorid, TE atype, ARG... args2)
		{
			func(aactorid, atype);
			func(aactorid, args2...);
		}
	};
}//namespace ngl