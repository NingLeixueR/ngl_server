#pragma once

#include "tools.h"

#include <iostream>

namespace ngl
{
	/*
		辅助缺省参数: template_arg<TF,TARG...>
		TF需要实现static func<T>(TARG...)
	*/

	struct np_arg_null
	{
		def_portocol(np_arg_null)
	};

	template <typename TF, typename ...TARG>
	class template_arg
	{
		template_arg() = delete;
		template_arg(const template_arg&) = delete;
		template_arg& operator=(const template_arg&) = delete;

		template <typename T>
		static void func2(TARG... args)
		{
			TF::template func<T>(args...);
		}
	public:
		template <typename T = np_arg_null, typename ...ARG>
		static void func(TARG... args)
		{
			if constexpr (std::is_same<T, np_arg_null>())
			{
				return;
			}
			func2<T>(args...);
			if constexpr (sizeof...(ARG) > 1)
			{
				func<ARG...>(args...);
			}
			if constexpr (sizeof...(ARG) == 1)
			{
				func2<ARG...>(args...);
			}
		}
	};


	template <typename TF, typename ...TARG>
	class template_arg_s
	{
		template_arg_s() = delete;
		template_arg_s(const template_arg_s&) = delete;
		template_arg_s& operator=(const template_arg_s&) = delete;

		static void func2(TARG... args)
		{
			TF::func(args...);
		}
	public:
		template <typename ...ARG>
		static void func(TARG... args, ARG... args2)
		{
			func2(args...);
			if constexpr (sizeof...(ARG) <= sizeof...(TARG))
			{
				func2(args2...);
			}
			else
			{
				func(args2...);
			}
		}
	};
}//namespace ngl