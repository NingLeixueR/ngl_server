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
// File overview: Declares interfaces for net.

#pragma once

#include "tools/serialize/pack.h"
#include "tools/threadtools.h"

namespace ngl
{
	class node_pack
	{
		node_pack() = delete;

		std::shared_ptr<pack>	m_pack		= nullptr; // Typed pack ownership.
		std::shared_ptr<void>	m_packvoid	= nullptr; // Type-erased pack ownership.
		bool					m_ispack	= false;  // True when m_pack is active, false when m_packvoid is active.
		i32_sessionid			m_sessionid = 0;   // Target session for the queued send.
	public:
		node_pack(i32_sessionid asessionid, std::shared_ptr<pack>& apack) :
			m_pack(apack),
			m_ispack(true),
			m_sessionid(asessionid)
		{}

		node_pack(i32_sessionid asessionid, std::shared_ptr<void>& apack) :
			m_packvoid(apack),
			m_ispack(false),
			m_sessionid(asessionid)
		{}

		~node_pack() = default;

		inline bool is_pack()
		{
			return m_ispack;
		}

		inline i32_sessionid sessionid()
		{
			return m_sessionid;
		}

		inline std::shared_ptr<pack>& get_pack()
		{
			return m_pack;
		}

		inline std::shared_ptr<void>& get_voidpack()
		{
			return m_packvoid;
		}

		inline pack* get()
		{
			if (m_ispack)
			{
				return m_pack.get();
			}
			else
			{
				return (pack*)m_packvoid.get();
			}
		}
	};
}// namespace ngl
