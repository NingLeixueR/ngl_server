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

#include <functional>
#include <cstdint>
#include <atomic>

namespace ngl
{
	class bpool
	{
	public:
		char* malloc(int32_t alen);
		void free(char* ap, int32_t alen);
	};

	class segpack;

	struct pack
	{
		pack() {};

		void free();
	public:
		// # 用来区分是协议类型
		ENET_PROTOCOL	m_protocol		= ENET_TCP;		// 消息类型
		i32_session		m_id			= 0;			// socket id
		pack_head		m_head;							// 包头
		segpack*		m_segpack		= nullptr;		// 残包使用
		bpool*			m_bpool			= nullptr;		// 用于包内存分配
		/*[如果包来自网络 Start]*/
		char*			m_buff			= nullptr;		// 包原始数据
		int32_t			m_len			= 0;			// 包原始数据字节数
		int32_t			m_pos			= 0;			// 包已读取数据字节数
		/*[如果包来自网络 Finish]*/

		~pack();

		// # 设置内存池
		void set(bpool& apool);
		
		// # 重置数据包
		void reset();

		// # 是否接收完毕
		bool isready();

		// # 为数据包分配
		bool malloc(int32_t alen);

		// # 在数据包中设置actor
		void set_actor(i64_actorid aactor, i64_actorid arequestactorid);

		// # 分配包
		static std::shared_ptr<pack> make_pack(bpool* apool, int32_t alen);
	};
}// namespace ngl