#include "ranklist.h"

namespace ngl
{
	ranklist::ranklist()
	{
		m_ranks[pbdb::eranklist::lv] = make_rank::make(pbdb::eranklist::lv);
		m_ranks[pbdb::eranklist::gold] = make_rank::make(pbdb::eranklist::gold);
	}

	void ranklist::set_id()
	{
		m_id = -1;
	}

	data_modified<pbdb::db_ranklist>* ranklist::get_rank(i64_actorid arankid)
	{
		auto itor = data().find(arankid);
		if (itor == data().end())
		{
			return nullptr;
		}
		return &itor->second;
	}

	bool ranklist::update_value(pbdb::eranklist atype, rank_item& litem, const pbdb::db_brief& abrief, bool afirstsynchronize)
	{
		data_modified<pbdb::db_ranklist>* lpdb_ranklist = get_rank(abrief.m_id());
		if (lpdb_ranklist == nullptr)
		{
			pbdb::db_ranklist* lpdata = get(abrief.m_id());
			lpdata->set_m_id(abrief.m_id());
			lpdb_ranklist = get_rank(abrief.m_id());
			assert(lpdb_ranklist != nullptr);
		}
		litem.init(abrief, lpdb_ranklist);
		return true;
	}

	bool ranklist::update_value(const pbdb::db_brief& abrief, bool afirstsynchronize)
	{
		rank_item litem;
		litem.m_actorid = abrief.m_id();
		std::map<pbdb::eranklist, bool> lupdatearr;
		bool lupdate = false;
		for (const auto& [_ranktype, _] : m_ranks)
		{
			lupdatearr[_ranktype] = update_value(_ranktype, litem, abrief, afirstsynchronize);
			lupdate = lupdate || lupdatearr[_ranktype];
		}
		if (lupdate)
		{
			rank_item& ldata = m_maprankitem[abrief.m_id()];
			for (const auto& [_ranktype, _] : m_ranks)
			{
				if (lupdatearr[_ranktype])
				{
					m_ranks[_ranktype]->erase(&ldata);
				}
			}
			ldata = litem;
			for (const auto& [_ranktype, _] : m_ranks)
			{
				if (lupdatearr[_ranktype])
				{
					m_ranks[_ranktype]->insert(&ldata);
				}
			}
		}
		return true;
	}

	void ranklist::add_data(const pbdb::db_ranklist& aitem)
	{
		rank_item& ltempitem = m_maprankitem[aitem.m_id()];
		ltempitem.m_actorid = aitem.m_id();

		for (const std::pair<const int32_t, pbdb::rankitem>& ritem : aitem.m_items())
		{
			rank_pair& lpair = ltempitem.m_data[(pbdb::eranklist)ritem.first];
			lpair.m_time = ritem.second.m_time();
			lpair.m_value = ritem.second.m_value();
		}

		for (const auto& [_ranktype, _rank] : m_ranks)
		{
			rankset_base& lrank = *_rank.get();
			lrank.insert(&ltempitem);
		}
	}

	void ranklist::initdata()
	{
		log_error()->print("actor_ranklist###loaddb_finish {}", data());
		for (const std::pair<const nguid, data_modified<pbdb::db_ranklist>>& item : data())
		{
			const pbdb::db_ranklist& ltemp = item.second.getconst();
			add_data(ltemp);
		}

		tdb_brief::nsp_cli<actor_ranklist>::getInstance().set_changedata_fun([this](int64_t aid, const pbdb::db_brief& abrief, bool afirstsynchronize)
			{
				update_value(abrief, afirstsynchronize);				
			});
	}

	pbdb::db_ranklist* ranklist::get(i64_actorid aactorid)
	{
		auto lprank = get_rank(aactorid);
		if (lprank == nullptr)
		{
			pbdb::db_ranklist ldbranklist;
			ldbranklist.set_m_id(aactorid);
			add(aactorid, ldbranklist);
		}
		return &data()[aactorid].get();
	}

	std::shared_ptr<pbnet::PROBUFF_NET_RANKLIST_RESPONSE> ranklist::get_ranklist(i64_actorid aroleid, pbdb::eranklist atype, int32_t apage)
	{
		auto pro = std::make_shared<pbnet::PROBUFF_NET_RANKLIST_RESPONSE>();
		pro->set_m_type(atype);
		pro->set_m_page(apage);
		if (m_ranks.contains(atype))
		{
			int32_t lcount = m_ranks[atype]->getpage(aroleid, apage, [&pro](int32_t aindex, const rank_item* aitem)
				{
					const pbdb::db_brief* lpbrief = tdb_brief::nsp_cli<actor_ranklist>::getInstance().getconst(aitem->m_actorid);
					if (lpbrief != nullptr)
					{
						*pro->add_m_items() = *lpbrief;
					}
				});
			pro->set_m_count(lcount);
			pro->set_m_rolerank(m_ranks[atype]->role_rank(aroleid));
		}
		else
		{
			pro->set_m_count(0);
			pro->set_m_rolerank(-1);
		}		
		return pro;
	}

	void ranklist::sync_ranklist(i64_actorid aroleid, pbdb::eranklist atype, int32_t aactivityid, int32_t apage)
	{
		auto pro = get_ranklist(aroleid, atype, apage);
		actor::send_client(aroleid, pro);
	}

	void ranklist::get_rank(int32_t arankid, std::vector<int64_t>& arolerank)
	{
		if (m_ranks.contains((pbdb::eranklist)arankid))
		{
			m_ranks[(pbdb::eranklist)arankid]->foreach([&arolerank](int32_t aindex, const rank_item* aitem)
				{
					arolerank.push_back(aitem->m_actorid);
				});
		}
	}

	void ranklist::remove_rank(int32_t arankid)
	{
		auto itor = m_ranks.find((pbdb::eranklist)arankid);
		if (itor == m_ranks.end())
		{
			return;
		}
		m_ranks.erase((pbdb::eranklist)arankid);
		erase(arankid);
	}

	void ranklist::add_rank(int32_t arankid)
	{
		std::unique_ptr<rankset_base> ltemp = make_rank::make((pbdb::eranklist)arankid);
		if (ltemp == nullptr)
		{
			return;
		}
		m_ranks[(pbdb::eranklist)arankid] = std::move(ltemp);
		auto& lrank = m_ranks[(pbdb::eranklist)arankid];
		for (auto itor = m_maprankitem.begin(); itor != m_maprankitem.end(); ++itor)
		{
			lrank->insert(&itor->second);
		}
	}
}//namespace ngl