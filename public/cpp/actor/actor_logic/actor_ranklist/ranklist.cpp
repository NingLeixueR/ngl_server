/*
* Copyright (c) [2020-2025] NingLeixueR
* 
* 项目名称：ngl_server
* 项目地址：https://github.com/NingLeixueR/ngl_server
* 
* 本文件是 ngl_server 项目的一部分，遵循 MIT 开源协议发布。
* 您可以按照协议规定自由使用、修改和分发本项目，包括商业用途，
* 但需保留原始版权和许可声明。
* 
* 许可详情参见项目根目录下的 LICENSE 文件：
* https://github.com/NingLeixueR/ngl_server/blob/main/LICENSE
*/
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
		return litem.init(atype, abrief, &get(abrief.mid()));
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
		for (const auto& item : data())
		{
			const pbdb::db_ranklist* lpdbranklist = item.second.getconst();
			if (lpdbranklist == nullptr)
			{
				continue;
			}
			add_data(*lpdbranklist);
		}


		std::set<i32_fieldnumber> lfieldset;
		tdb_brief::nsp_cread<actor_ranklist>::instance_readall((actor_ranklist*)get_actor(), pb_field::field_number<pbdb::db_brief>(lfieldset, "mid", "mbase", "mactivityvalues"));

		tdb_brief::nsp_cread<actor_ranklist>::instance(get_actor()->id_guid()).set_changedatafun(
			[this](int64_t aid, const pbdb::db_brief& abrief, bool afirstsynchronize)
			{
				log_error()->print("##[lv : {}#{}]", abrief.mid(), abrief.mbase().mlv());
				update_value(abrief, afirstsynchronize);
			}
		);
	}

	bool ranklist::ranklist_get(i64_actorid aroleid, pbdb::eranklist atype, int32_t apage, pbnet::PROBUFF_NET_RANKLIST_RESPONSE& apro)
	{
		apro.set_mtype(atype);
		apro.set_mpage(apage);
		if (m_ranks.contains(atype))
		{
			int32_t lcount = m_ranks[atype]->getpage(aroleid, apage, [&apro,this](int32_t aindex, const rank_item* aitem)
				{
					const pbdb::db_brief* lpbrief = tdb_brief::nsp_cread<actor_ranklist>::instance(get_actor()->id_guid()).getconst(aitem->m_actorid);
					if (lpbrief != nullptr)
					{
						*apro.add_mitems() = *lpbrief;
					}
				});
			apro.set_mcount(lcount);
			apro.set_mrolerank(m_ranks[atype]->role_rank(aroleid));
		}
		else
		{
			apro.set_mcount(0);
			apro.set_mrolerank(-1);
		}		
		return true;
	}

	void ranklist::ranklist_sync(i64_actorid aroleid, pbdb::eranklist atype, int32_t aactivityid, int32_t apage)
	{
		pbnet::PROBUFF_NET_RANKLIST_RESPONSE pro;
		i64_actorid lbriefroleid = nguid::make_type(aroleid, ACTOR_BRIEF);
		if (!ranklist_get(lbriefroleid, atype, apage, pro))
		{
			return;
		}
		actor::send_client(aroleid, pro);
	}

	void ranklist::rank_get(int32_t arankid, std::vector<int64_t>& arolerank)
	{
		if (m_ranks.contains((pbdb::eranklist)arankid))
		{
			m_ranks[(pbdb::eranklist)arankid]->foreach([&arolerank](int32_t aindex, const rank_item* aitem)
				{
					arolerank.push_back(aitem->m_actorid);
				});
		}
	}

	void ranklist::rank_remove(int32_t arankid)
	{
		auto itor = m_ranks.find((pbdb::eranklist)arankid);
		if (itor == m_ranks.end())
		{
			return;
		}
		m_ranks.erase((pbdb::eranklist)arankid);
		erase(arankid);
	}

	void ranklist::rank_add(int32_t arankid)
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