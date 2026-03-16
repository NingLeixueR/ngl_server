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

#include "tools/serialize/pack_head.h"
#include "tools/type.h"

#include <cstdint>
#include <functional>
#include <memory>

namespace ngl
{
	class bpool
	{
	public:
		char* malloc(int32_t alen);
		void free(char* ap, int32_t alen);
	};

	struct pack
	{
		pack() = default;

		void free();
	public:
		ENET_PROTOCOL	m_protocol = ENET_TCP;
		i32_session		m_id = 0;
		pack_head		m_head;
		bpool*			m_bpool = nullptr;
		bool			m_rate_accounted = false;
		char*			m_buff = nullptr;
		int32_t			m_len = 0;
		int32_t			m_pos = 0;

		~pack();

		void set(bpool& apool);
		void reset();
		bool isready();
		bool malloc(int32_t alen);
		void set_actor(i64_actorid aactor, i64_actorid arequestactorid);

		static std::shared_ptr<pack> make_pack(bpool* apool, int32_t alen);
	};
}// namespace ngl