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
		i32_session m_id				= 0;
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
		bool isok();
		bool malloc(int alen);

		void set_actor(i64_actorid aactor, i64_actorid arequestactorid);

		static std::shared_ptr<pack> make_pack(bpool* apool, int alen);
	};

}