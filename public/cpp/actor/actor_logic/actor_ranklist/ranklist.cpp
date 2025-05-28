#include "ranklist.h"

namespace ngl
{


	ranklist::ranklist()
	{
		m_ranks[pbdb::eranklist::lv] = std::make_unique<rankset<pbdb::eranklist::lv>>();
		m_ranks[pbdb::eranklist::lv]->set_count(ttab_specialid::m_ranklistmaxcount);

		create_activity_rank<pbdb::eranklist::activity_lv + 1001>();
	}

	void ranklist::set_id()
	{
		m_id = -1;
	}

	data_modified<pbdb::db_ranklist>* ranklist::get_rank(i64_actorid aroleid)
	{
		auto itor = data().find(aroleid);
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
			auto& lmap = *lpdata->mutable_m_items();
			for (int i = 0; i < pbdb::eranklist::count; ++i)
			{
				lmap[i].set_m_time(litem.m_data[(pbdb::eranklist)i].m_time);
				lmap[i].set_m_value(litem.m_data[(pbdb::eranklist)i].m_value);
			}
			lpdb_ranklist = get_rank(abrief.m_id());
			assert(lpdb_ranklist != nullptr);
		}
		litem.init(abrief, lpdb_ranklist);

		return true;
	}

	bool ranklist::update_value(const pbdb::db_brief& abrief, bool afirstsynchronize)
	{
		std::map<i64_actorid, pbdb::db_brief>& ldata = tdb_brief::nsp_cli<actor_ranklist>::getInstance().m_data;
		rank_item litem;
		litem.m_actorid = abrief.m_id();
		bool lupdatearr[pbdb::eranklist::count] = { false };
		bool lupdate = false;
		for (int i = 0; i < pbdb::eranklist::count; ++i)
		{
			lupdatearr[i] = update_value(pbdb::eranklist::lv, litem, abrief, afirstsynchronize);
			lupdate = lupdate || lupdatearr[i];
		}
		if (lupdate)
		{
			rank_item& ldata = m_maprankitem[abrief.m_id()];
			for (int i = 0; i < pbdb::eranklist::count; ++i)
			{
				if (lupdatearr[i])
				{
					m_ranks[(pbdb::eranklist)i]->erase(&ldata);
				}
			}
			ldata = litem;
			for (int i = 0; i < pbdb::eranklist::count; ++i)
			{
				if (lupdatearr[i])
				{
					m_ranks[(pbdb::eranklist)i]->insert(&ldata);
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

		for (int32_t i = 0; i < pbdb::eranklist::count; ++i)
		{
			rankset_base& lrank = *m_ranks[(pbdb::eranklist)i].get();
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
				/*for (const auto& item : abrief.m_activityvalues().m_activity_rolelv())
				{
					pbdb::eranklist ltype = (pbdb::eranklist)(pbdb::activity_lv + item.first);
					if (!m_ranks.contains(ltype))
					{
						m_ranks[ltype] = create_rankset::create(pbdb::activity_lv, aid);
						m_ranks[ltype]->set_count(ttab_specialid::m_ranklistmaxcount);
					}
				}*/
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
		return pro;
	}

	void ranklist::sync_ranklist(i64_actorid aroleid, pbdb::eranklist atype, int32_t aactivityid, int32_t apage)
	{
		auto pro = get_ranklist(aroleid, atype, apage);
		actor::send_client(aroleid, pro);
	}
}//namespace ngl