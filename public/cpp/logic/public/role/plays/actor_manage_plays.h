#pragma once

#include "actor.h"
#include "map.h"

namespace ngl
{
	class actor_manage_plays : public actor
	{
		actor_manage_plays();
	public:
		friend class actor_instance<actor_manage_plays>;
		static actor_manage_plays& getInstance()
		{
			return actor_instance<actor_manage_plays>::instance();
		}

		virtual void init()
		{
		}
		static void nregister();

		virtual ~actor_manage_plays() {}

		virtual void loaddb_finish(bool adbishave);

		// 创建玩法
		bool handle(const message<pbnet::PROBUFF_NET_MATCHING_SUCCESS>& adata);

		// 定时器
		bool timer_handle(const message<timerparm>&);
	};
}

