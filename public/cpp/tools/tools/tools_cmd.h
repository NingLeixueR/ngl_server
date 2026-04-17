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
// File overview: Command-line argument parser and interactive console command dispatcher.

#pragma once

#include <functional>
#include <list>
#include <map>

namespace ngl::tools
{
	///////////////////////////////////////////////////////////
	// Used to key/std::function<void(ARG...)>structure
	// Through functionpush keyandstd::function<void(ARG...)>
	// Through function functione xecutecorresponding
	// TAG,, used togenerate type
	// TKEYused tofind keytype
	// ...ARG used tospecifiedstd::function<void(ARG...)> parameters
	template <typename TAG, typename TKEY, typename ...ARG>
	class cmd
	{
		cmd() = delete;
		cmd(const cmd&) = delete;
		cmd& operator=(const cmd&) = delete;

		static std::map<TKEY, std::function<void(ARG...)>> m_fun;
	public:
		static bool empty()
		{
			return m_fun.empty();
		}

		static bool function(const TKEY& akey, ARG... arg)
		{
			auto itor = m_fun.find(akey);
			if (itor == m_fun.end())
			{
				return false;
			}
			itor->second(arg...);
			return true;
		}

		static std::function<void(ARG...)>& add(const TKEY& akey)
		{
			return m_fun[akey];
		}
	};

	template <typename TAG, typename TKEY, typename ...ARG>
	std::map<TKEY, std::function<void(ARG...)>> cmd<TAG, TKEY, ARG...>::m_fun;
}//namespace ngl
