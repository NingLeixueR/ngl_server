#pragma once

#include "nactor_auto.h"
#include "db.pb.h"


namespace ngl
{
	struct rank_item
	{
		i64_actorid m_actorid						= nguid::make(); // actor id
		int64_t	m_values[pbdb::eranklist::count]	= {0};			 // 属性值
		int32_t	m_time[pbdb::eranklist::count]		= {0};			 // 刷新时间

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
					return abrief.m_lv();
				});
		}

		void change(pbdb::eranklist atype, pbdb::db_ranklist& aranklist)
		{
			pbdb::rankitem& ltemp = (*aranklist.mutable_m_items())[atype];
			ltemp.set_m_time(m_time[atype]);
			ltemp.set_m_value(m_values[atype]);
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
		ecompare value_compare(pbdb::eranklist atype, const rank_item& ar)
		{
			ecompare ltype = value_compare(m_values[atype], ar.m_values[atype]);
			if (atype == pbdb::eranklist::lv)
			{//值越大排名越高
				return ltype;
			}
			else
			{
				return (ecompare)(-(int32_t)ltype);
			}
		}

		bool compare(pbdb::eranklist atype, const rank_item& ar)
		{
			ecompare ltype = value_compare(atype, ar);
			if (ltype == eless_equal)
			{
				ltype = (ecompare)(-(int32_t)value_compare(atype, ar));
				if (ltype == eless_equal)
				{
					return m_actorid < ar.m_actorid;
				}
			}
			return ltype == eless_bigger ? true : false;
		}

		bool equal_value(pbdb::eranklist atype, const rank_item& ar)
		{
			return m_values[atype] == ar.m_values[atype];
		}
	};
}//namespace ngl