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
// File overview: Implements resource consumption tracking and cost validation.


#include "tools/tools/tools_consume.h"
#include "actor/protocol/nprotocol.h"

namespace ngl::tools
{
	consume::consume(std::string&& aname) :
		m_name(std::move(aname)),
		m_beg(0)
	{}

	consume::consume(const std::string& aname) :
		m_name(aname),
		m_beg(0)
	{
	}

	void consume::start()
	{
		m_beg = time_wheel::getms();
	}

	void consume::finish()
	{
		int64_t lconsuming = time_wheel::getms() - m_beg;
		bool lerror = lconsuming > sysconfig::consumings();
		// Slow operations log at error level so they stand out in production traces.
		(lerror ? log_error() : log_info())->print("time consuming [{}:{}]", m_name, lconsuming);
	}
}//namespace ngl
