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
// File overview: Template argument pack utilities for variadic template metaprogramming.

#pragma once

#include "tools/serialize/ndefine.h"
#include "tools/tools.h"
#include "tools/type.h"

#include <iostream>

namespace ngl::tools
{
	/*
		Parameters: template_arg<TF,TARG...>
		TFneed to static func<T>(TARG...)
	*/
	template <typename TF, typename ...TARG>
	struct template_arg
	{
		template <typename ...ARG>
		static void func(TARG... args)
		{
			(TF::template func<ARG>(args...), ...);
		}
	};
}//namespace ngl::tools
