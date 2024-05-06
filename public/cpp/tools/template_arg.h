#pragma once

#include "tools.h"

#include <iostream>

namespace ngl
{
	template <typename TF, typename ...TARG>
	class template_arg
	{
	public:
		template <typename T>
		static void func(const TARG&... args, T*)
		{
			TF::template func<T>(args...);
		}

		template <typename T, typename ...ARG>
		static void func(const TARG&... args)
		{
			func<T>(args..., nullptr);
			if constexpr (sizeof...(ARG) > 1)
			{
				func<ARG...>(args...);
			}
			if constexpr (sizeof...(ARG) == 1)
			{
				func<ARG...>(args..., nullptr);
			}
		}
	};

	class test_arg
	{
	public:
		template <typename T>
		static void func(bool abool)
		{
			std::cout << dtype_name(T) << std::endl;
		}
	};
	class test_arg2
	{
	public:
		template <typename T>
		static void func()
		{
			std::cout << dtype_name(T) << std::endl;
		}
	};

	void test_template_arg();
}