#pragma once

#include "nactor_auto.h"
#include "rank_item.h"
#include "db.pb.h"

#include <iostream>
#include <sstream>
#include <list>

namespace ngl
{
	class actor_ranklist;

	template <pbdb::eranklist RTYPE>
	class operator_value
	{
	public:
		// >
		bool operator()(rank_item* al, rank_item* ar)const
		{
			return al->bigger(RTYPE, *ar);
		}
	};

	class ranklist : public tdb_ranklist::db_modular
	{
		ranklist(const ranklist&) = delete;
		ranklist& operator=(const ranklist&) = delete;

		std::map<i64_actorid, rank_item> m_data;

		class rankset_base
		{
			int32_t m_count;
		public:
			rankset_base() :
				m_count(-1)
			{}

			virtual void erase(rank_item* aitem) = 0;
			virtual void insert(rank_item* aitem) = 0;
			virtual void foreach(const std::function<void(int32_t, const rank_item*)>&) = 0;
			virtual int32_t getpage(int32_t apage, int32_t aevernmu, const std::function<void(int32_t, const rank_item*)>& afun) = 0;
			virtual void set_count(int32_t acount)
			{
				m_count = acount;
			}
			virtual int32_t count()
			{
				return m_count;
			}
			virtual void check() = 0;
		};

		template <pbdb::eranklist ETYPE>
		class rankset :public rankset_base
		{
			std::set<rank_item*, operator_value<ETYPE>> m_rankdata;
		public:
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

			virtual int32_t getpage(int32_t apage, int32_t aevernmu, const std::function<void(int32_t, const rank_item*)>& afun)
			{
				int32_t lbegindex = (apage-1)* aevernmu;
				int32_t lendindex = lbegindex + aevernmu;
				if (lbegindex < 0 || lbegindex > m_rankdata.size())
				{
					return m_rankdata.size();
				}
				auto itor = m_rankdata.begin();
				std::advance(itor, lbegindex);
				for (int lindex = 1; itor != m_rankdata.end() && lindex<= aevernmu;++lindex,++itor)
				{
					afun(lindex, *itor);
				}
				return m_rankdata.size();
			}

			void check()
			{
				int32_t lcount = count();
				if (lcount < 0)
				{
					return;
				}
				while (lcount < m_rankdata.size())
				{
					m_rankdata.erase(std::prev(m_rankdata.end()));
				}
			}
		};

		std::unique_ptr<rankset_base> m_ranks[pbdb::eranklist::count];
	public:
		ranklist()
		{
			m_ranks[pbdb::eranklist::lv] = std::make_unique<rankset<pbdb::eranklist::lv>>();
			m_ranks[pbdb::eranklist::lv]->set_count(10);
		}

		virtual void set_id()
		{
			m_id = -1;
		}

		std::map<nguid, data_modified<pbdb::db_ranklist>>* get_ranklist()
		{
			return &data();
		}

		const pbdb::db_ranklist* get_constrank(i64_actorid aroleid)
		{
			auto itor = data().find(aroleid);
			if (itor == data().end())
			{
				return nullptr;
			}
			return &itor->second.getconst();
		}

		pbdb::db_ranklist* get_rank(i64_actorid aroleid, bool achange = true)
		{
			auto itor = data().find(aroleid);
			if (itor == data().end())
			{
				return nullptr;
			}
			return &itor->second.get(achange);
		}

		bool update_value(pbdb::eranklist atype, rank_item& litem, const pbdb::db_brief& abrief, bool afirstsynchronize)
		{
			const pbdb::db_ranklist* lpconstdata = find(abrief.m_id());
			litem.init(abrief, lpconstdata);

			if (lpconstdata == nullptr)
			{
				pbdb::db_ranklist* lpdata = get(abrief.m_id());
				lpdata->set_m_id(abrief.m_id());
				auto& lmap = *lpdata->mutable_m_items();
				for (int i = 0; i < pbdb::eranklist::count; ++i)
				{
					lmap[i].set_m_time(litem.m_time[i]);
					lmap[i].set_m_value(litem.m_values[i]);
				}
			}
			
			auto itor = m_data.find(abrief.m_id());
			if (itor != m_data.end())
			{
				if (litem.equal_value(atype, itor->second) == false)
				{
					litem.m_time[atype] = (int32_t)localtime::gettime();
					pbdb::db_ranklist* lpdata = get(abrief.m_id());
					litem.change(atype, *lpdata);
					return true;
				}
			}
			else
			{
				m_data.insert(std::make_pair(abrief.m_id(), litem));
				return true;
			}
			return false;
		}

		bool update_value(const pbdb::db_brief& abrief, bool afirstsynchronize)
		{
			std::map<i64_actorid, pbdb::db_brief>& ldata = tdb_brief::nsp_cli<actor_ranklist>::m_data;
			rank_item litem;
			bool lupdatearr[pbdb::eranklist::count] = { false };
			bool lupdate = false;
			for (int i = 0; i < pbdb::eranklist::count; ++i)
			{
				lupdatearr[i] = update_value(pbdb::eranklist::lv, litem, abrief, afirstsynchronize);
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
							m_ranks[i]->insert(&ldata);
							m_ranks[i]->check();
						}
					}
				}
			}
			return true;
		}

		void add_data(const pbdb::db_ranklist& aitem)
		{
			rank_item& ltempitem = m_data[aitem.m_id()];
			ltempitem.m_actorid = aitem.m_id();

			for (const std::pair<const int32_t, pbdb::rankitem>& ritem : aitem.m_items())
			{
				ltempitem.m_time[(pbdb::eranklist)ritem.first] = ritem.second.m_time();
				ltempitem.m_values[(pbdb::eranklist)ritem.first] = ritem.second.m_value();
			}

			for (int32_t i = 0; i < pbdb::eranklist::count; ++i)
			{
				rankset_base& lrank = *m_ranks[i].get();
				lrank.insert(&ltempitem);
				m_ranks[i]->check();
			}
		}

		virtual void initdata()
		{
			log_error()->print("actor_ranklist###loaddb_finish");
			rankset_base& lrank = *m_ranks[pbdb::eranklist::lv].get();

			for (const std::pair<const nguid, data_modified<pbdb::db_ranklist>>& item : data())
			{
				const pbdb::db_ranklist& ltemp = item.second.getconst();
				add_data(ltemp);
			}

			tdb_brief::nsp_cli<actor_ranklist>::set_changedata_fun([this](int64_t aid, const pbdb::db_brief& abrief, bool afirstsynchronize)
				{
					update_value(abrief, afirstsynchronize);
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
		
		std::shared_ptr<pbnet::PROBUFF_NET_RANKLIST_RESPONSE> get_ranklist(pbdb::eranklist atype, int32_t apage, int32_t aeverynum)
		{
			auto pro = std::make_shared<pbnet::PROBUFF_NET_RANKLIST_RESPONSE>();
			pro->set_m_type(atype);
			pro->set_m_page(apage);
			pro->set_m_everynum(aeverynum);
			int32_t lcount = m_ranks[atype]->getpage(apage, aeverynum, [&pro](int32_t aindex, const rank_item* aitem)
				{
					const pbdb::db_brief* lpbrief = tdb_brief::nsp_cli<actor_ranklist>::getconst(aitem->m_actorid);
					if (lpbrief != nullptr)
					{
						*pro->add_m_items() = *lpbrief;
					}
				});
			pro->set_m_count(lcount);
			return pro;
		}

		void sync_ranklist(i64_actorid aroleid, pbdb::eranklist atype, int32_t apage, int32_t aeverynum)
		{
			auto pro = get_ranklist(atype, apage, aeverynum);
			actor::send_client(aroleid, pro);
		}
	};
}// namespace ngl