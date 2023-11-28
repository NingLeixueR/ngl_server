#pragma once

#include "actor_plays.h"


namespace ngl
{
	class actor_plays_go_undergroundpalace : public actor_plays
	{

	public:
		actor_plays_go_undergroundpalace(ENUM_ACTOR atype, int32_t adataid, void* data);

		virtual ~actor_plays_go_undergroundpalace() {}

		static void actor_register();

		enum { ACTOR_TYPE = ACTOR_PLAYS_GO_UNDERGROUNDPALACE};

		// 初始化玩法后调用
		virtual void init_after() {}
		// 玩家进入
		virtual void enter(i64_actorid aroleid) {}
		// 玩家离开
		virtual void leave(i64_actorid aroleid) {}
		// 准备阶段[开始or结束]
		virtual void preparation(bool astart) {}
		// 玩法阶段[开始or结束]
		virtual void play(bool astart) {}
		// 结算阶段[开始or结束]
		virtual void settlement(bool astart) {}
		// 玩家死亡
		virtual void player_death(i64_actorid aroleid) {}
		// 进入视野
		virtual void player_enterview(i64_actorid aroleid, std::set<i64_actorid>& aother) {}
		// 离开视野
		virtual void player_leaveview(i64_actorid aroleid1, std::set<i64_actorid>& aother) {}

	};
}