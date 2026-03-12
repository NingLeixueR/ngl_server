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
