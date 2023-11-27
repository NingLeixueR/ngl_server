#include "plays.h"


namespace ngl
{
	void plays::init(plays_pram& apram)
	{
		m_tab = allcsv::tab<tab_plays>(apram.m_playstid);
		assert(m_tab != nullptr);
		init_after(apram);
		// 开始准备阶段
		m_stage = estage_preparation;
		preparation(true);
	}

	void plays::stage(estage astage, bool astart)
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

	bool plays::change_stage(estage astage)
	{
		if (m_stage <= astage)
			return false;
		stage(m_stage, false);
		m_stage = astage;
		m_begtm = localtime::gettime();
		stage(m_stage, true);
	}

	void plays::update()
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
}