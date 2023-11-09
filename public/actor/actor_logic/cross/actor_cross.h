#pragma once

#include "actor.h"

namespace ngl
{
	class actor_cross : public actor
	{
		actor_cross() :
			actor(
				actorparm
				{
					.m_parm
					{
						.m_type = ACTOR_CROSS,
						.m_area = actor_guid::none_area(),// 跨服此值无意义
						.m_id = nconfig::m_nodeid
					},
					.m_weight = 0x7fffffff,
				})
		{
		}
	public:
		friend class actor_instance<actor_cross>;
		static actor_cross& getInstance()
		{
			return actor_instance<actor_cross>::instance();
		}
		
		virtual void init()
		{
			//m_info.set(this);
		}
		static void actor_register();

		virtual ~actor_cross() {}

		virtual void loaddb_finish(bool adbishave);

		enum { ACTOR_TYPE = ACTOR_CROSS};

		// CMD 协议
		//bool handle(i32_threadid athread, const std::shared_ptr<pack>& apack, pbnet::PROBUFF_NET_CMD& adata);

		// 定时器
		bool timer_handle(i32_threadid athread, const std::shared_ptr<pack>& apack, timerparm& adata);

	private:
	};

	


}

