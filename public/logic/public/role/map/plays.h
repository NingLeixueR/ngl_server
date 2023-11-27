#pragma once

#include "manage_csv.h"
#include "type.h"
#include "actor_protocol.h"

namespace ngl
{
	// [plays_pram] 可扩展
	struct plays_pram
	{
		int m_playstid;							// 玩法id
		std::vector<i64_actorid> m_players;		// 玩家列表

		//##可根据具体玩法扩展

		def_portocol(plays_pram, m_playstid, m_players);
	};

	class actor_plays;
	class plays
	{
	protected:
		enum estage
		{
			estage_preparation,		// 准备阶段
			estage_play,			// 玩法阶段
			estage_settlement,		// 结算阶段
		};
		estage m_stage;
		tab_plays* m_tab;
		int32_t m_begtm;// 阶段开始时间
	public:
		void init(plays_pram& apram);

		void stage(estage astage, bool astart);

		bool change_stage(estage astage);
		// 初始化玩法后调用
		virtual void init_after(plays_pram& apram) {}
		// 玩家进入
		virtual void enter(i64_actorid aroleid) {}
		// 准备阶段[开始or结束]
		virtual void preparation(bool astart) {}
		// 玩法阶段[开始or结束]
		virtual void play(bool astart) {}
		// 结算阶段[开始or结束]
		virtual void settlement(bool astart) {}

		void update();
	};
}