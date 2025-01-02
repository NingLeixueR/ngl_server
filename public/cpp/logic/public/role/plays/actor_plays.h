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
			estage_preparation,		// ׼���׶�
			estage_play,			// �淨�׶�
			estage_settlement,		// ����׶�
			estage_over,			// ����
		};
		estage m_stage;			// �淨�׶�
		int32_t m_begtm;		// �׶ο�ʼʱ��
		tab_plays* m_tab;
		std::map<int64_t, std::shared_ptr<unit_role>> get_constrole;

	public:
		actor_plays(ENUM_ACTOR atype, int32_t adataid, void* data);

		virtual void init();

		static void nregister();

		virtual ~actor_plays() {}

		virtual void loaddb_finish(bool adbishave) {}

		// ���ý׶�
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

		// �ı�׶�
		bool change_stage(estage astage)
		{
			if (m_stage <= astage)
			{
				return false;
			}
			stage(m_stage, false);
			m_stage = astage;
			m_begtm = localtime::gettime();
			stage(m_stage, true);
			return true;
		}

		bool check_stage(estage astage)
		{
			return m_stage == astage;
		}

		void update_stage()
		{
			int32_t lnow = localtime::gettime();
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

		// ��ʼ���淨�����
		virtual void init_after() {}
		// ��ҽ���
		virtual void enter(i64_actorid aroleid) {}
		// ����뿪
		virtual void leave(i64_actorid aroleid) {}
		// ׼���׶�[��ʼor����]
		virtual void preparation(bool astart) {}
		// �淨�׶�[��ʼor����]
		virtual void play(bool astart) {}
		// ����׶�[��ʼor����]
		virtual void settlement(bool astart) {}
		// �������
		virtual void player_death(i64_actorid aroleid) {}
		// ������Ұ
		virtual void player_enterview(i64_actorid aroleid, std::set<i64_actorid>& aother) {}
		// �뿪��Ұ
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
			int lindex = std::rand() % m_tab->m_birthpoint.size();
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
				// ɾ��actor
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