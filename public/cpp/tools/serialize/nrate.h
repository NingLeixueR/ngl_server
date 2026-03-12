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

#include "tools/tools.h"

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
			const int lnow = (int)localtime::gettime();
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
