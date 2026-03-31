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

#include <utility>

#include "tools/tools/tools_thread.h"
#include "tools/serialize/pack.h"

namespace ngl
{
	class node_pack
	{
		std::shared_ptr<pack>	m_pack		= nullptr; // Typed pack ownership.
		std::shared_ptr<void>	m_packvoid	= nullptr; // Type-erased pack ownership.
	public:
		node_pack()
		{
		}

		explicit node_pack(std::shared_ptr<pack> apack) :
			m_pack(std::move(apack))
		{}

		explicit node_pack(std::shared_ptr<void> apack) :
			m_packvoid(std::move(apack))
		{}

		~node_pack() = default;

		inline const std::shared_ptr<pack>& get_pack()
		{
			if (m_packvoid == nullptr && m_pack == nullptr)
			{
				tools::no_core_dump();
			}
			if (m_pack != nullptr)
			{
				return m_pack;
			}
			m_pack = std::static_pointer_cast<pack>(m_packvoid);
			return m_pack;
		}

		inline pack* get()
		{
			return get_pack().get();
		}
	};
}// namespace ngl
