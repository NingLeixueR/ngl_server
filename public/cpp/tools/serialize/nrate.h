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
			int m_resetutc = 0;		// 上次重置的utc
			int m_ratecount = 0;	// 收到数据包的数量

			interval() :
				m_resetutc((int)localtime::gettime()), m_ratecount(0)
			{}

			void reset()
			{
				m_resetutc = (int)localtime::gettime();
				m_ratecount = 0;
			}
		};
		std::unordered_map<i32_socket, interval> m_data;
	public:
		bool add(i32_socket aid)
		{
			int lnow = (int)localtime::gettime();
			auto lpinterval = tools::findmap(m_data, aid);
			if (lpinterval == nullptr)
			{
				m_data.insert(std::make_pair(aid, interval()));
				return true;
			}
			if (lpinterval->m_resetutc == 0)
			{
				lpinterval->reset();
				return true;
			}
			if (lnow >= lpinterval->m_resetutc + sysconfig::rate_interval())
			{
				lpinterval->reset();
				return true;
			}
			++lpinterval->m_ratecount;
			if (sysconfig::rate_count() < lpinterval->m_ratecount)
			{
				return false;
			}
			return true;
		}
	};
}//namespace ngl