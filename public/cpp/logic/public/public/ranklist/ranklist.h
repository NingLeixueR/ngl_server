#pragma once

#include "nactor_auto.h"
#include "db.pb.h"

#include <iostream>
#include <sstream>
#include <list>

namespace ngl
{
	class actor_ranklist;

	struct rank_item
	{
		i64_actorid m_actorid;
		int64_t		m_values[pbdb::eranklist::count];
		int32_t		m_time[pbdb::eranklist::count];

		rank_item() :
			m_actorid(0),
			m_values{0},
			m_time{0}
		{}

		const pbdb::db_brief* get()const
		{
			return tdb_brief::nsp_cli<actor_ranklist>::getconst(m_actorid);
		}

		void init(const pbdb::db_brief& abrief, const pbdb::db_ranklist& aranklist)
		{
			const auto& lmap = aranklist.m_items();
			{//pbdb::eranklist::lv
				auto itor = lmap.find((int32_t)pbdb::eranklist::lv);
				if (itor == lmap.end())
				{
					m_time[pbdb::eranklist::lv] = localtime::gettime();
				}
				else
				{
					m_time[pbdb::eranklist::lv] = itor->second.m_time();
				}
				m_values[pbdb::eranklist::lv] = abrief.m_lv();
			}
			m_actorid = abrief.m_id();
		}

		void change(pbdb::eranklist atype, pbdb::db_ranklist& aranklist)
		{
			pbdb::rankitem& ltemp = (*aranklist.mutable_m_items())[atype];
			ltemp.set_m_time(m_time[atype]);
			ltemp.set_m_value(m_values[atype]);
		}
		
		bool less(pbdb::eranklist atype, const rank_item& ar)
		{
			if (m_values[atype] < ar.m_values[atype])
			{
				return true;
			}
			else if (m_values[atype] > ar.m_values[atype])
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

	template <pbdb::eranklist RTYPE>
	class operator_value
	{
	public:
		// <
		bool operator()(rank_item* al, rank_item* ar)const
		{
			return al->less(RTYPE, *ar);
		}
	};

	class ranklist : public tdb_ranklist::db_modular
	{
		ranklist(const ranklist&) = delete;
		ranklist& operator=(const ranklist&) = delete;

	

		std::map<i64_actorid, rank_item> m_data;

		class rank_set_base
		{
		public:
			virtual const rank_item* find(rank_item* aitem) = 0;
			virtual void erase(rank_item* aitem) = 0;
			virtual void insert(rank_item* aitem) = 0;
			virtual void foreach(const std::function<void(int32_t, const rank_item*)>&) = 0;
		};

		template <pbdb::eranklist ETYPE>
		class rank_set :public rank_set_base
		{
			std::set<rank_item*, operator_value<ETYPE>> m_rankdata;
		public:
			virtual const rank_item* find(rank_item* aitem)
			{
				auto itor = m_rankdata.find(aitem);
				if (itor == m_rankdata.end())
					return nullptr;
				return *itor;
			}

			virtual void erase(rank_item* aitem)
			{
				m_rankdata.erase(aitem);
			}

			virtual void insert(rank_item* aitem)
			{
				m_rankdata.insert(aitem);
			}

			virtual void foreach(const std::function<void(int32_t, const rank_item*)>& afun)
			{
				int32_t lindex = 0;
				for (const rank_item* item : m_rankdata)
				{
					afun(lindex++, item);
				}
			}
		};

		std::unique_ptr<rank_set_base> m_ranks[pbdb::eranklist::count];
	public:
		ranklist()
		{
			m_ranks[pbdb::eranklist::lv] = std::make_unique<rank_set<pbdb::eranklist::lv>>();
		}

		virtual void set_id()
		{
			m_id = -1;
		}

		std::map<nguid, data_modified<pbdb::db_ranklist>>* get_ranklist()
		{
			return &data();
		}

		
		bool update_value(pbdb::eranklist atype, rank_item& litem, const pbdb::db_brief& abrief)
		{
			const pbdb::db_ranklist* lpdata = find(abrief.m_id());
			litem.init(abrief, *lpdata);
			auto itor = m_data.find(abrief.m_id());
			if (itor != m_data.end())
			{
				if (litem.equal_value(atype, itor->second) == false)
				{
					litem.m_time[atype] = localtime::gettime();
					pbdb::db_ranklist* lpdata = get(abrief.m_id());
					litem.change(atype, *lpdata);
					return true;
				}
			}
			return false;
		}

		virtual void initdata()
		{
			log_error()->print("actor_ranklist###loaddb_finish");
			tdb_brief::nsp_cli<actor_ranklist>::set_recv_data_finish([this](const pbdb::db_brief& abrief)
				{
					std::map<i64_actorid, pbdb::db_brief>& ldata = tdb_brief::nsp_cli<actor_ranklist>::m_data;
					rank_item litem;
					bool lupdatearr[pbdb::eranklist::count] = { false };
					bool lupdate = false;
					for (int i = 0; i < pbdb::eranklist::count; ++i)
					{
						lupdatearr[i] = update_value(pbdb::eranklist::lv, litem, abrief);
						lupdate = lupdate || lupdatearr[i];
					}
					if (lupdate)
					{
						rank_item& ldata = m_data[abrief.m_id()];
						for (int i = 0; i < pbdb::eranklist::count; ++i)
						{
							if (lupdatearr[i])
							{
								m_ranks[i]->erase(&ldata);
							}
						}
						ldata = litem;
						for (int i = 0; i < pbdb::eranklist::count; ++i)
						{
							if (lupdatearr[i])
							{
								if (i == pbdb::eranklist::lv)
								{
									m_ranks[i]->erase(&ldata);
								}
							}
						}
					}
				});
		}

		const pbdb::db_ranklist* find(i64_actorid aactorid)
		{
			auto itor = get_ranklist()->find(aactorid);
			if (itor == get_ranklist()->end())
			{
				return nullptr;
			}
			return &itor->second.getconst();
		}

		pbdb::db_ranklist* get(i64_actorid aactorid)
		{
			if (get_ranklist()->find(aactorid) == get_ranklist()->end())
			{
				pbdb::db_ranklist ldbranklist;
				ldbranklist.set_m_id(aactorid);
				add(aactorid, ldbranklist);
			}
			return &(*get_ranklist())[aactorid].get();
		}
		
		std::shared_ptr<pbnet::PROBUFF_NET_RANKLIST_RESPONSE> get_ranklist(pbdb::eranklist atype)
		{
			auto pro = std::make_shared<pbnet::PROBUFF_NET_RANKLIST_RESPONSE>();
			pro->set_m_type(atype);
			m_ranks[atype]->foreach([&pro](int32_t aindex, const rank_item* aitem)
				{
					const pbdb::db_brief* lpbrief = tdb_brief::nsp_cli<actor_ranklist>::getconst(aitem->m_actorid);
					*pro->add_m_items() = *lpbrief;
				});
			return pro;
		}

		void sync_ranklist(i64_actorid aroleid, pbdb::eranklist atype)
		{
			auto pro = get_ranklist(atype);
			actor::send_client(aroleid, pro);
		}
	};
}// namespace ngl