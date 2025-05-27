#pragma once

#include "nactor_auto.h"
#include "db.pb.h"


namespace ngl
{
	struct rank_pair
	{
		int64_t m_value = 0;
		int32_t m_time = 0;
	};
	struct rank_item
	{
		i64_actorid m_actorid = nguid::make();					// actor id
		std::map<pbdb::eranklist, rank_pair> m_data;			// 属性值/刷新时间

		rank_item() :
			m_actorid(0)
		{}

		const pbdb::db_brief* get()const
		{
			return tdb_brief::nsp_cli<actor_ranklist>::getconst(m_actorid);
		}

		void init(
			const pbdb::db_brief& abrief, 
			data_modified<pbdb::db_ranklist>* aranklist, 
			pbdb::eranklist atype, 
			const std::function<int64_t(const pbdb::db_brief&)>& avalfun
		)
		{
			m_data[atype].m_value = avalfun(abrief);

			auto& lmap = aranklist->getconst().m_items();
			auto itor = lmap.find(atype);
			if (itor != lmap.end())
			{
				if (itor->second.m_value() != m_data[atype].m_value)
				{
					change(atype, aranklist->get());
				}
				else
				{
					m_data[atype].m_time = itor->second.m_time();
				}
				return;
			}
			change(atype, aranklist->get());
		}

		void init(const pbdb::db_brief& abrief, data_modified<pbdb::db_ranklist>* aranklist)
		{
			init(abrief, aranklist, pbdb::eranklist::lv, [](const pbdb::db_brief& abrief)
				{
					return abrief.m_lv();
				});

			init(abrief, aranklist, (pbdb::eranklist)(pbdb::eranklist::activity_lv + 1001), [](const pbdb::db_brief& abrief)
				{
					auto itor = abrief.m_activityvalues().m_activity_rolelv().find(pbdb::eranklist::activity_lv + 1001);
					if (itor == abrief.m_activityvalues().m_activity_rolelv().end())
					{
						return -1;
					}
					return itor->second;
				});
		}

		void change(pbdb::eranklist atype, pbdb::db_ranklist& aranklist)
		{
			pbdb::rankitem& ltemp = (*aranklist.mutable_m_items())[atype];
			m_data[atype].m_time = localtime::gettime();
			ltemp.set_m_time(m_data[atype].m_time);
			ltemp.set_m_value(m_data[atype].m_value);
		}

		enum ecompare
		{
			eless_bigger	= 1,		// 大于
			eless_less		= -1,		// 小于
			eless_equal		= 0,		// 等于
		};

		ecompare value_compare(int64_t al, int64_t ar)
		{
			if (al > ar)
			{
				return eless_bigger;
			}
			else if (al < ar)
			{
				return eless_less;
			}
			return eless_equal;
		}

		// 值比较 
		ecompare value_compare(pbdb::eranklist atype, rank_item& ar)
		{
			auto itor = ar.m_data.find(atype);
			ecompare ltype = value_compare(m_data[atype].m_value, ar.m_data[atype].m_value);
			if (atype == pbdb::eranklist::lv 
				|| atype == pbdb::eranklist::activity_lv + 1001
				|| atype == pbdb::eranklist::activity_gold + 1001
				)
			{//值越大排名越高
				return ltype;
			}
			else
			{
				return (ecompare)(-(int32_t)ltype);
			}
		}

		ecompare time_compare(pbdb::eranklist atype, rank_item& ar)
		{
			ecompare ltype = value_compare(m_data[atype].m_time, ar.m_data[atype].m_time);
			return (ecompare)(-(int32_t)ltype);
		}

		bool compare(pbdb::eranklist atype, rank_item& ar)
		{
			ecompare ltype = value_compare(atype, ar);
			if (ltype == eless_equal)
			{
				ltype = time_compare(atype, ar);
				if (ltype == eless_equal)
				{
					return m_actorid < ar.m_actorid;
				}
			}
			return ltype == eless_bigger ? true : false;
		}

		bool equal_value(pbdb::eranklist atype, rank_item& ar)
		{
			return m_data[atype].m_value == ar.m_data[atype].m_value;
		}

		int64_t value(pbdb::eranklist atype)
		{
			return m_data[atype].m_value;
		}
	};
}//namespace ngl