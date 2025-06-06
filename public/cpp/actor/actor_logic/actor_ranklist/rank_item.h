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

		const pbdb::db_brief* get()const;

		bool init(
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
					return true;
				}
				else
				{
					m_data[atype].m_time = itor->second.m_time();
					return false;
				}				
			}
			change(atype, aranklist->get());
			return true;
		}

		template <int ACTIVITYID>
		int64_t activitylv(const pbdb::db_brief& abrief)
		{
			auto itor = abrief.m_activityvalues().m_activity_rolelv().find(pbdb::eranklist::activity_lv + ACTIVITYID);
			if (itor == abrief.m_activityvalues().m_activity_rolelv().end())
			{
				return -1;
			}
			return itor->second;
		};

		template <int ACTIVITYID>
		int64_t activitygold(const pbdb::db_brief& abrief)
		{
			auto itor = abrief.m_activityvalues().m_activity_rolegold().find(pbdb::eranklist::activity_gold + ACTIVITYID);
			if (itor == abrief.m_activityvalues().m_activity_rolegold().end())
			{
				return -1;
			}
			return itor->second;
		};

		bool init(pbdb::eranklist atype, const pbdb::db_brief& abrief, data_modified<pbdb::db_ranklist>* aranklist)
		{
			switch (atype)
			{
			case pbdb::eranklist::lv:
				return init(abrief, aranklist, pbdb::eranklist::lv, [](const pbdb::db_brief& abrief)
					{
						return abrief.m_lv();
					});
			case pbdb::eranklist::gold:
				return init(abrief, aranklist, pbdb::eranklist::gold, [](const pbdb::db_brief& abrief)
					{
						return abrief.m_moneygold();
					});
			case pbdb::eranklist::activity_lv + 1:
				return init(abrief, aranklist, (pbdb::eranklist)(pbdb::eranklist::activity_lv + 1),
					std::bind(&rank_item::activitylv<1>, this, std::placeholders::_1)
				);
			case pbdb::eranklist::activity_gold + 1:
				return init(abrief, aranklist, (pbdb::eranklist)(pbdb::eranklist::activity_gold + 1),
					std::bind(&rank_item::activitygold<1>, this, std::placeholders::_1)
				);
			}
			return false;
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
				|| atype == pbdb::eranklist::gold
				|| atype == pbdb::eranklist::activity_lv + 1
				|| atype == pbdb::eranklist::activity_gold + 1
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