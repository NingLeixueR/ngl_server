#pragma once

#include "nactor_auto.h"
#include "db.pb.h"


namespace ngl
{
	struct rank_item
	{
		i64_actorid m_actorid;
		int64_t		m_values[pbdb::eranklist::count];
		int32_t		m_time[pbdb::eranklist::count];

		rank_item() :
			m_actorid(0),
			m_values{ 0 },
			m_time{ 0 }
		{}

		const pbdb::db_brief* get()const
		{
			return tdb_brief::nsp_cli<actor_ranklist>::getconst(m_actorid);
		}

		void init(const pbdb::db_brief& abrief, const pbdb::db_ranklist* aranklist, pbdb::eranklist atype, const std::function<int64_t(const pbdb::db_brief&)>& avalfun)
		{
			m_values[atype] = avalfun(abrief);

			if (aranklist != nullptr)
			{
				auto& lmap = aranklist->m_items();
				auto itor = lmap.find((int32_t)atype);
				if (itor != lmap.end())
				{
					m_time[atype] = itor->second.m_time();
					return;
				}
			}
			m_time[atype] = (int32_t)localtime::gettime();
		}

		void init(const pbdb::db_brief& abrief, const pbdb::db_ranklist* aranklist)
		{
			m_actorid = abrief.m_id();
			init(abrief, aranklist, pbdb::eranklist::lv, [](const pbdb::db_brief& abrief)
				{
					return  abrief.m_lv();
				});
		}

		void change(pbdb::eranklist atype, pbdb::db_ranklist& aranklist)
		{
			pbdb::rankitem& ltemp = (*aranklist.mutable_m_items())[atype];
			ltemp.set_m_time(m_time[atype]);
			ltemp.set_m_value(m_values[atype]);
		}

		bool bigger(pbdb::eranklist atype, const rank_item& ar)
		{
			if (m_values[atype] > ar.m_values[atype])
			{
				return true;
			}
			else if (m_values[atype] < ar.m_values[atype])
			{
				return false;
			}
			else
			{
				if (m_time[atype] < ar.m_time[atype])
				{
					return true;
				}
				else if (m_time[atype] > ar.m_time[atype])
				{
					return false;
				}
				else
				{
					return m_actorid < ar.m_actorid;
				}
			}
		}

		bool equal_value(pbdb::eranklist atype, const rank_item& ar)
		{
			return m_values[atype] == ar.m_values[atype];
		}

	};
}//namespace ngl