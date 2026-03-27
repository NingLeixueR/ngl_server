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
#include "tools/tools/tools_thread.h"

namespace ngl
{
	class node_pack
	{
		node_pack() = delete;

		std::shared_ptr<pack>	m_pack		= nullptr; // Typed pack ownership.
		std::shared_ptr<void>	m_packvoid	= nullptr; // Type-erased pack ownership.
	public:
		explicit node_pack(std::shared_ptr<pack>& apack) :
			m_pack(apack)
		{}

		explicit node_pack(std::shared_ptr<void>& apack) :
			m_packvoid(apack)
		{}

		~node_pack() = default;

		inline bool is_pack() const
		{
			return m_pack != nullptr;
		}

		inline const std::shared_ptr<pack>& get_pack() const
		{
			return m_pack;
		}

		inline const std::shared_ptr<void>& get_voidpack() const
		{
			return m_packvoid;
		}

		inline pack* get() const
		{
			if (is_pack())
			{
				return m_pack.get();
			}
			else
			{
				return(pack*)(m_packvoid != nullptr ? m_packvoid.get() : nullptr);
			}
		}
	};
}// namespace ngl
