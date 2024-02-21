#pragma once

#include <cstdint>
#include <functional>
#include <atomic>
#include "type.h"
#include "pack_head.h"

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
		// 用来区分是协议类型
		ENET_PROTOCOL m_protocol	= ENET_TCP;

		i32_session m_id			= 0;
		pack_head m_head;
		segpack* m_segpack			= nullptr;
		bpool* m_bpool				= nullptr;
		////如果包来自网络///
		char* m_buff				= nullptr;
		int m_len					= 0;
		int m_pos					= 0;

		~pack();

		void set(bpool& apool);
		void reset();
		bool isready();
		bool malloc(int alen);
		// 在数据包中设置actor
		void set_actor(i64_actorid aactor, i64_actorid arequestactorid);
		// 分配包
		static std::shared_ptr<pack> make_pack(bpool* apool, int alen);
	};

}