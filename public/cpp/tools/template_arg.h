#pragma once

#include "tools.h"
#include "type.h"
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