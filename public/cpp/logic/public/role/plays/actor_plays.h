#pragma once

#include "actor.h"
#include "map.h"


namespace ngl
{
	class actor_plays : public actor
	{
	protected:
		aoimap m_map;
		enum estage
		{
			estage_preparation,		// 准备阶段
			estage_play,			// 玩法阶段
			estage_settlement,		// 结算阶段
			estage_over,			// 结束
		};
		estage m_stage;			// 玩法阶段
		int32_t m_begtm;		// 阶段开始时间
		tab_plays* m_tab;
		std::map<int64_t, std::shared_ptr<unit_role>> get_constrole;

	public:
		actor_plays(ENUM_ACTOR atype, int32_t adataid, void* data);

		virtual void init();

		static void nregister();

		virtual ~actor_plays() {}

		virtual void loaddb_finish(bool adbishave) {}

		// 设置阶段
		void stage(estage astage, bool astart)
		{
			switch (astage)
			{
			case estage_preparation:
				preparation(astart);
				break;
			case estage_play:
				play(astart);
				break;
			case estage_settlement:
				settlement(astart);
				break;
			}
		}

		// 改变阶段
		bool change_stage(estage astage)
		{
			if (m_stage <= astage)
			{
				return false;
			}
			stage(m_stage, false);
			m_stage = astage;
			m_begtm = (int32_t)localtime::gettime();
			stage(m_stage, true);
			return true;
		}

		bool check_stage(estage astage)
		{
			return m_stage == astage;
		}

		void update_stage()
		{
			int32_t lnow = (int32_t)localtime::gettime();
			if (m_stage == estage_preparation)
			{
				if (lnow <= m_begtm + m_tab->m_preparation_tm)
				{
					change_stage(estage_play);
				}
			}
			else if (m_stage == estage_play)
			{
				if (lnow <= m_begtm + m_tab->m_play_tm)
				{
					change_stage(estage_settlement);
				}
			}
			else if (m_stage == estage_settlement)
			{
				if (lnow <= m_begtm + m_tab->m_settlement_tm)
				{
					settlement(false);
				}
			}
		}

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

		bool handle(const message<mforward<pbnet::PROBUFF_NET_ENTER_PLAYS>>& adata)
		{
			auto lparm = adata.get_data();
			if (get_constrole.find(lparm->identifier()) != get_constrole.end())
			{
				return true;
			}
			if (m_tab->m_birthpoint.empty())
			{
				log_error()->print("actor_plays::handle(pbnet::PROBUFF_NET_ENTER_PLAYS) m_birthpoint.empty()");
				return true;
			}
			int lindex = tools::rand() % m_tab->m_birthpoint.size();
			auto& item = get_constrole[lparm->identifier()];
			item.reset(new unit_role(lparm->identifier()));
			m_map.enter(item.get(), m_tab->m_birthpoint[lindex].m_x, m_tab->m_birthpoint[lindex].m_y);
			enter(lparm->identifier());
			return true;
		}
		
		bool timer_handle(const message<timerparm>& adata)
		{
			update_stage();
			if (check_stage(estage_over) == true)
			{
				// 删除actor
				actor_base::erase_actor_byid(id_guid());
			}
			else
			{
				m_map.update(adata.get_data()->m_ms);
			}
			return true;
		}
	};
}