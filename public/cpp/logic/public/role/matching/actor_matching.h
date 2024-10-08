#pragma once

#include "nactor_auto.h"
#include "nsp_server.h"
#include "nsp_client.h"
#include "matching.h"
#include "actor.h"

namespace ngl
{
	// ### 匹配玩法
	class actor_matching : public actor
	{
		actor_matching();

		matching m_matching;
	public:
		using nclient = tdb_brief::nsp_cli<actor_matching>;

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
		bool handle(const message<mforward<pbnet::PROBUFF_NET_MATCHING>>& adata);
		// 取消玩法
		bool handle(const message<mforward<pbnet::PROBUFF_NET_MATCHING_CANCEL>>& adata);
		// 发起确认
		bool handle(const message<mforward<pbnet::PROBUFF_NET_MATCHING_CONFIRM>>& adata);
		// 玩法已经建立完成
		bool handle(const message<pbnet::PROBUFF_NET_MATCHING_SUCCESS_RESPONSE>& adata);
		
		bool timer_handle(const message<timerparm>& adata);
	};
}// namespace ngl