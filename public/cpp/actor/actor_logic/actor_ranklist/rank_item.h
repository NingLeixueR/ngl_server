#pragma once

#include "nactor_auto.h"
#include "db.pb.h"


namespace ngl
{
	struct rank_item
	{
		i64_actorid m_actorid						= nguid::make(); // actor id
		int64_t	m_values[pbdb::eranklist::count]	= {0};			 // ����ֵ
		int32_t	m_time[pbdb::eranklist::count]		= {0};			 // ˢ��ʱ��

		rank_item() :
			m_actorid(0),
			m_values{ 0 },
			m_time{ 0 }
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
			m_values[atype] = avalfun(abrief);

			auto& lmap = aranklist->getconst().m_items();
			auto itor = lmap.find(atype);
			if (itor != lmap.end())
			{
				if (itor->second.m_value() != m_values[atype])
				{
					change(atype, aranklist->get());
				}
				else
				{
					m_time[atype] = itor->second.m_time();
				}
				return;
			}
			m_time[atype] = localtime::gettime();
		}

		void init(const pbdb::db_brief& abrief, data_modified<pbdb::db_ranklist>* aranklist)
		{
			init(abrief, aranklist, pbdb::eranklist::lv, [](const pbdb::db_brief& abrief)
				{
					return abrief.m_lv();
				});
		}

		void change(pbdb::eranklist atype, pbdb::db_ranklist& aranklist)
		{
			pbdb::rankitem& ltemp = (*aranklist.mutable_m_items())[atype];
			m_time[atype] = localtime::gettime();
			ltemp.set_m_time(m_time[atype]);
			ltemp.set_m_value(m_values[atype]);
		}

		enum ecompare
		{
			eless_bigger	= 1,		// ����
			eless_less		= -1,		// С��
			eless_equal		= 0,		// ����
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

		// ֵ�Ƚ� 
		ecompare value_compare(pbdb::eranklist atype, const rank_item& ar)
		{
			ecompare ltype = value_compare(m_values[atype], ar.m_values[atype]);
			if (atype == pbdb::eranklist::lv)
			{//ֵԽ������Խ��
				return ltype;
			}
			else
			{
				return (ecompare)(-(int32_t)ltype);
			}
		}

		ecompare time_compare(pbdb::eranklist atype, const rank_item& ar)
		{
			ecompare ltype = value_compare(m_time[atype], ar.m_time[atype]);
			return (ecompare)(-(int32_t)ltype);
		}

		bool compare(pbdb::eranklist atype, const rank_item& ar)
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

		bool equal_value(pbdb::eranklist atype, const rank_item& ar)
		{
			return m_values[atype] == ar.m_values[atype];
		}

		int64_t value(pbdb::eranklist atype)
		{
			return m_values[atype];
		}
	};
}//namespace ngl