#pragma once

#include "actor.h"
#include "actor_roleitem.h"
#include "matching.h"

namespace ngl
{
	// ### 匹配玩法
	class actor_matching : public actor
	{
		actor_matching();

		matching m_matching;

	public:
		using type_roleitems = roleitems<actor_matching, pbdb::db_brief>;
		friend class actor_instance<actor_matching>;
		static actor_matching& getInstance()
		{
			return actor_instance<actor_matching>::instance();
		}

		virtual void init(){}

		static void nregister();

		virtual ~actor_matching() {}

		virtual void loaddb_finish(bool adbishave) {}

		// 匹配玩法
		bool handle(message<mforward<pbnet::PROBUFF_NET_MATCHING>>& adata);
		// 取消玩法
		bool handle(message<mforward<pbnet::PROBUFF_NET_MATCHING_CANCEL>>& adata);
		// 发起确认
		bool handle(message<mforward<pbnet::PROBUFF_NET_MATCHING_CONFIRM>>& adata);
		// 玩法已经建立完成
		bool handle(message<pbnet::PROBUFF_NET_MATCHING_SUCCESS_RESPONSE>& adata);
		
		bool timer_handle(message<timerparm>& adata);
	};

}