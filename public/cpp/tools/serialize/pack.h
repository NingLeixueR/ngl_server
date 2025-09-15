#pragma once

#include "pack_head.h"
#include "type.h"

#include <functional>
#include <cstdint>
#include <atomic>

namespace ngl
{
	class bpool
	{
	public:
		bpool();
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