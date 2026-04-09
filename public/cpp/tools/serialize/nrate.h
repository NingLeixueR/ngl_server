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
// File overview: Declares interfaces for serialize.


#pragma once

#include "tools/tab/xml/sysconfig.h"
#include "tools/tools.h"
#include "tools/type.h"

#include <unordered_map>

namespace ngl
{
	class nrate
	{
		struct interval
		{
			int m_resetutc = 0;
			int m_ratecount = 0;

			interval() = default;

			explicit interval(int anow, int acount = 1) :
				m_resetutc(anow),
				m_ratecount(acount)
			{}

			void reset(int anow, int acount = 1)
			{
				m_resetutc = anow;
				m_ratecount = acount;
			}
		};

		std::unordered_map<i32_socket, interval> m_data;
	public:
		bool add(i32_socket aid)
		{
			const int lnow = (int)tools::time::gettime();
			auto [itor, inserted] = m_data.try_emplace(aid, interval(lnow));
			if (inserted)
			{
				return true;
			}

			interval& linterval = itor->second;
			if (linterval.m_resetutc == 0)
			{
				linterval.reset(lnow);
				return true;
			}
			if (lnow >= linterval.m_resetutc + sysconfig::rate_interval())
			{
				linterval.reset(lnow);
				return true;
			}

			++linterval.m_ratecount;
			return linterval.m_ratecount <= sysconfig::rate_count();
		}

		void erase(i32_socket aid)
		{
			m_data.erase(aid);
		}
	};
}//namespace ngl
