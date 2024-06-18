#pragma once

#include "tools.h"

#include <iostream>

namespace ngl
{
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
}//namespace ngl