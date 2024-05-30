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
		char* malloc(int alen);
		void free(char* ap, int alen);
	};

	class segpack;

	struct pack
	{
		pack() {};

		void free();
	public:
		// # 用来区分是协议类型
		ENET_PROTOCOL	m_protocol	= ENET_TCP;

		i32_session		m_id			= 0;
		pack_head		m_head;
		segpack*		m_segpack		= nullptr;
		bpool*			m_bpool			= nullptr;
		////如果包来自网络 START///
		char*			m_buff			= nullptr;
		int				m_len			= 0;
		int				m_pos			= 0;
		////如果包来自网络 FINISH///

		~pack();

		// # 设置内存池
		void set(bpool& apool);
		
		// # 重置数据包
		void reset();

		// # 是否接收完毕
		bool isready();

		// # 为数据包分配
		bool malloc(int alen);

		// # 在数据包中设置actor
		void set_actor(i64_actorid aactor, i64_actorid arequestactorid);

		// # 分配包
		static std::shared_ptr<pack> make_pack(bpool* apool, int alen);
	};
}// namespace ngl