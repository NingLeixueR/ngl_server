#include "ranklist.h"

namespace ngl
{
	ranklist::ranklist()
	{
		m_ranks[pbdb::eranklist::lv] = make_rank::make(pbdb::eranklist::lv);
		m_ranks[pbdb::eranklist::gold] = make_rank::make(pbdb::eranklist::gold);
	}

	void ranklist::related_actorid()
	{
		set_actorid(nguid::make());
	}

	bool ranklist::update_value(pbdb::eranklist atype, rank_item& litem, const pbdb::db_brief& abrief, bool afirstsynchronize)
	{
		data_modified<pbdb::db_ranklist>* lpdb_ranklist = find(abrief.mid());
		if (lpdb_ranklist == nullptr)
		{
			pbdb::db_ranklist* lpdata = get(abrief.mid());
			lpdata->set_mid(abrief.mid());
			lpdb_ranklist = find(abrief.mid());
			if (lpdb_ranklist == nullptr)
			{
				tools::no_core_dump();
				return true;
			}
		}
		return litem.init(atype, abrief, lpdb_ranklist);
	}

	bool ranklist::update_value(const pbdb::db_brief& abrief, bool afirstsynchronize)
	{
		rank_item litem;
		litem.m_actorid = abrief.mid();
		std::map<pbdb::eranklist, bool> lupdatearr;
		bool lupdate = false;
		for (const auto& [_ranktype, _] : m_ranks)
		{
			lupdatearr[_ranktype] = update_value(_ranktype, litem, abrief, afirstsynchronize);
			lupdate = lupdate || lupdatearr[_ranktype];
		}
		if (lupdate)
		{
			rank_item& ldata = m_maprankitem[abrief.mid()];
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
		rank_item& ltempitem = m_maprankitem[aitem.mid()];
		ltempitem.m_actorid = aitem.mid();

		for (const std::pair<const int32_t, pbdb::rankitem>& ritem : aitem.mitems())
		{
			rank_pair& lpair = ltempitem.m_data[(pbdb::eranklist)ritem.first];
			lpair.m_time = ritem.second.mtime();
			lpair.m_value = ritem.second.mvalue();
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

		tdb_brief::nsp_cli<actor_ranklist>::instance(get_actor()->id_guid()).set_changedata_fun(
			[this](int64_t aid, const pbdb::db_brief& abrief, bool afirstsynchronize)
			{
				update_value(abrief, afirstsynchronize);				
			});
	}

	pbdb::db_ranklist* ranklist::get(i64_actorid aactorid)
	{
		data_modified<pbdb::db_ranklist>* lprank = find(aactorid);
		if (lprank == nullptr)
		{
			pbdb::db_ranklist ldbranklist;
			ldbranklist.set_mid(aactorid);
			add(aactorid, ldbranklist);
		}
		return &data()[aactorid].get();
	}

	std::shared_ptr<pbnet::PROBUFF_NET_RANKLIST_RESPONSE> ranklist::get_ranklist(i64_actorid aroleid, pbdb::eranklist atype, int32_t apage)
	{
		auto pro = std::make_shared<pbnet::PROBUFF_NET_RANKLIST_RESPONSE>();
		pro->set_mtype(atype);
		pro->set_mpage(apage);
		if (m_ranks.contains(atype))
		{
			int32_t lcount = m_ranks[atype]->getpage(aroleid, apage, [&pro,this](int32_t aindex, const rank_item* aitem)
				{
					const pbdb::db_brief* lpbrief = tdb_brief::nsp_cli<actor_ranklist>::instance(
						get_actor()->id_guid()).getconst(aitem->m_actorid);
					if (lpbrief != nullptr)
					{
						*pro->add_mitems() = *lpbrief;
					}
				});
			pro->set_mcount(lcount);
			pro->set_mrolerank(m_ranks[atype]->role_rank(aroleid));
		}
		else
		{
			pro->set_mcount(0);
			pro->set_mrolerank(-1);
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