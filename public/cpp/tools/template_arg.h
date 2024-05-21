#pragma once

#include "tools.h"

#include <iostream>

namespace ngl
{
	template <typename TF, typename ...TARG>
	class template_arg
	{
		template <typename T>
		static void func2(TARG... args)
		{
			TF::template func<T>(args...);
		}
	public:
		template <typename T, typename ...ARG>
		static void func(TARG... args)
		{
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