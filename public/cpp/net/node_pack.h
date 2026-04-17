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
// File overview: Node-level packet envelope for cross-node message routing.

#pragma once

#include <deque>
#include <mutex>
#include <utility>

#include "tools/tools/tools_thread.h"
#include "tools/serialize/pack.h"

namespace ngl
{
	class node_pack
	{
		mutable std::shared_ptr<pack>			m_pack		= nullptr; // Typed pack ownership.
		std::shared_ptr<void>					m_packvoid	= nullptr; // Type-erased pack ownership.
		mutable std::shared_ptr<pack_head>		m_head		= nullptr;
	public:
		node_pack()
		{
		}

		explicit node_pack(std::shared_ptr<pack> apack) :
			m_pack(std::move(apack))
		{
			if (m_pack->m_head != nullptr)
			{
				m_head = std::make_shared<pack_head>(*m_pack->m_head);
			}
		}

		explicit node_pack(std::shared_ptr<void> apack) :
			m_packvoid(std::move(apack))
		{
			const std::shared_ptr<pack>& lpack = get_pack();
			if (lpack->m_head != nullptr)
			{
				m_head = std::make_shared<pack_head>(*lpack->m_head);
			}			
		}

		~node_pack() = default;

		inline const std::shared_ptr<pack>& get_pack()const 
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

		const char* buff()const
		{
			return get_pack()->m_buff;
		}

		int32_t pos()const 
		{
			return get_pack()->m_pos;
		}

		const std::shared_ptr<pack_head>& head()const 
		{
			return m_head;
		}

		const char* head_data()const
		{
			return (const char*)m_head->m_data;
		}

		int32_t head_byte()const
		{
			return sizeof(m_head->m_data);
		}
	};

	template <typename TITEM>
	class send_list
	{
		std::deque<TITEM>	m_list;				// Pending send queue drained by async completion handlers.
		std::mutex			m_mutex;
		bool				m_issend = false;	// Whether one async send is currently in flight.
	public:
		template <typename TVAL>
		void push(TVAL&& aitem)
		{
			std::lock_guard<std::mutex> llock(m_mutex);
			m_list.emplace_back(std::forward<TVAL>(aitem));
		}

		bool pop(TITEM& aitem, bool aasync)
		{
			std::lock_guard<std::mutex> llock(m_mutex);
			if (m_list.empty())
			{
				m_issend = false;
				return false;
			}
			if (m_issend && !aasync)
			{
				return false;
			}
			m_issend = true;
			aitem = std::move(m_list.front());
			m_list.pop_front();
			return true;
		}

		template <typename TFUN>
		bool send(bool aasync, TFUN&& afun)
		{
			TITEM litem;
			if (!pop(litem, aasync))
			{
				return false;
			}
			std::forward<TFUN>(afun)(litem);
			return true;
		}
	};

	class npack_list
	{
		send_list<std::shared_ptr<node_pack>>	m_list;
	public:

		template <typename T>
		void push(std::shared_ptr<T>& apack)
		{
			m_list.push(std::make_shared<node_pack>(apack));
		}

		bool pop(std::shared_ptr<node_pack>& apack, bool aasync)
		{
			return m_list.pop(apack, aasync);
		}

		template <typename TNET, typename TNDATA>
		void send(
			bool aasync,
			const std::shared_ptr<TNDATA>& adata,
			TNET* anet,
			void (TNET::* asendfun)(const std::shared_ptr<TNDATA>&, const std::shared_ptr<node_pack>&)
		)
		{
			m_list.send(aasync, [adata, anet, asendfun](const std::shared_ptr<node_pack>& anodepack)
				{
					(anet->*asendfun)(adata, anodepack);
				}
			);
		}
	};
}// namespace ngl
