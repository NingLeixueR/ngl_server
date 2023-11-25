#pragma once

#include "actor.h"
#include "map.h"


namespace ngl
{
	class actor_map : public actor
	{
		actor_map();

		aoimap m_map;
	public:
		friend class actor_instance<actor_map>;
		static actor_map& getInstance()
		{
			return actor_instance<actor_map>::instance();
		}

		virtual void init()
		{
			//m_info.set(this);
		}
		static void actor_register();

		virtual ~actor_map() {}

		virtual void loaddb_finish(bool adbishave);

		enum { ACTOR_TYPE = ACTOR_MAP};

		// CMD 协议
		//bool handle(i32_threadid athread, const std::shared_ptr<pack>& apack, pbnet::PROBUFF_NET_CMD& adata);


		// 定时器
		bool timer_handle(i32_threadid athread, const std::shared_ptr<pack>& apack, timerparm& adata);

	private:
	};




}

