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

#include "tools/serialize/nrate.h"
#include "tools/serialize/pack.h"
#include "tools/threadtools.h"
#include "tools/type.h"

#include <functional>
#include <memory>
#include <unordered_map>

namespace ngl
{
	class segpack_heartbeat
	{
	public:
		// Heartbeat packets are handled locally and never enter the protocol queue.
		static bool is_heartbeat(i32_protocolnum aprotocolnum);
	};

	class segpack
	{
		// Partially received packets are tracked per socket until both header and
		// body are complete.
		std::unordered_map<i32_socket, std::shared_ptr<pack>> m_data;
		bpool m_pool;
		nrate m_rate;

	public:
		enum class edopush
		{
			e_continue,
			e_break,
			e_error,
		};

		segpack() = default;

		// Pushes one receive fragment. Returns true only when the fragment has
		// been fully consumed or safely buffered for later completion.
		bool push(i32_socket aid, const char* ap, int alen, bool aislanip);
		void close(i32_socket aid);

	private:
		void erase(i32_socket aid);
		std::shared_ptr<pack> remnant_package(i32_socket aid);
		edopush telnet_cmd(std::shared_ptr<pack>& apack, const char*& ap, int& alen, bool aislanip);
		bool check_recv(std::shared_ptr<pack>& apack, i32_socket aid, int alen, bool aislanip);
		bool is_heartbeat(std::shared_ptr<pack>& apack);
		bool fill_pack(std::shared_ptr<pack>& apack, const char*& ap, int& alen, int& len);
		edopush do_push(i32_socket aid, const char*& ap, int& alen, bool aislanip);
	};
}// namespace ngl
