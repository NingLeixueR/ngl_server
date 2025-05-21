#include "ranklist.h"

namespace ngl
{
	ranklist::ranklist()
	{
		m_ranks[pbdb::eranklist::lv] = std::make_unique<rankset<pbdb::eranklist::lv>>();
		m_ranks[pbdb::eranklist::lv]->set_count(ttab_specialid::m_ranklistmaxcount);
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

		auto itor = m_maprankitem.find(abrief.m_id());
		if (itor != m_maprankitem.end())
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
			m_maprankitem.insert(std::make_pair(abrief.m_id(), litem));
			return true;
		}
		return false;
	}

	bool ranklist::update_value(const pbdb::db_brief& abrief, bool afirstsynchronize)
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
			rank_item& ldata = m_maprankitem[abrief.m_id()];
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

	void ranklist::add_data(const pbdb::db_ranklist& aitem)
	{
		rank_item& ltempitem = m_maprankitem[aitem.m_id()];
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

	void ranklist::initdata()
	{
		log_error()->print("actor_ranklist###loaddb_finish {}", data());
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

	const pbdb::db_ranklist* ranklist::find(i64_actorid aactorid)
	{
		auto itor = get_ranklist()->find(aactorid);
		if (itor == get_ranklist()->end())
		{
			return nullptr;
		}
		return &itor->second.getconst();
	}

	pbdb::db_ranklist* ranklist::get(i64_actorid aactorid)
	{
		if (get_ranklist()->find(aactorid) == get_ranklist()->end())
		{
			pbdb::db_ranklist ldbranklist;
			ldbranklist.set_m_id(aactorid);
			add(aactorid, ldbranklist);
		}
		return &(*get_ranklist())[aactorid].get();
	}

	std::shared_ptr<pbnet::PROBUFF_NET_RANKLIST_RESPONSE> ranklist::get_ranklist(pbdb::eranklist atype, int32_t apage)
	{
		auto pro = std::make_shared<pbnet::PROBUFF_NET_RANKLIST_RESPONSE>();
		pro->set_m_type(atype);
		pro->set_m_page(apage);
		int32_t lcount = m_ranks[atype]->getpage(apage, [&pro](int32_t aindex, const rank_item* aitem)
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

	void ranklist::sync_ranklist(i64_actorid aroleid, pbdb::eranklist atype, int32_t apage)
	{
		auto pro = get_ranklist(atype, apage);
		actor::send_client(aroleid, pro);
	}
}//namespace ngl