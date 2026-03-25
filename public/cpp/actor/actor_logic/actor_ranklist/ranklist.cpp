/*
* Copyright (c) [2020-2025] NingLeixueR
* 
* Project name: ngl_server
* Project URL: https://github.com/NingLeixueR/ngl_server
* 
* This file is part of the ngl_server project and is distributed under the MIT License.
* You may use, modify, and distribute this project under the license, including commercial use,
* but you must retain the original copyright and license notice.
* 
* For license details, see the LICENSE file in the project root:
* https://github.com/NingLeixueR/ngl_server/blob/main/LICENSE
*/
// File overview: Implements logic for actor ranklist.


#include "actor/actor_logic/actor_ranklist/ranklist.h"

#include <set>

namespace ngl
{
	ranklist::ranklist()
	{
		m_ranks.emplace(pbdb::eranklist::lv, make_rank::make(pbdb::eranklist::lv));
		m_ranks.emplace(pbdb::eranklist::gold, make_rank::make(pbdb::eranklist::gold));
	}

	void ranklist::related_actorid()
	{
		set_actorid(nguid::make());
	}

	bool ranklist::update_value(pbdb::eranklist atype, rank_item& litem, const pbdb::db_brief& abrief, [[maybe_unused]] bool afirstsynchronize)
	{
		return litem.init(atype, abrief, &get(abrief.mid()));
	}

	bool ranklist::update_value(const pbdb::db_brief& abrief, bool afirstsynchronize)
	{
		rank_item litem;
		litem.m_actorid = abrief.mid();
		std::set<pbdb::eranklist> lchangeds;
		bool lupdate = false;
		for (const auto& lpair : m_ranks)
		{
			const bool lchanged = update_value(lpair.first, litem, abrief, afirstsynchronize);
			if (lchanged)
			{
				lchangeds.emplace(lpair.first);
				lupdate = true;
			}
		}
		if (lupdate)
		{
			auto lit = m_maprankitem.try_emplace(abrief.mid()).first;
			rank_item& ldata = lit->second;
			for (const pbdb::eranklist ltype : lchangeds)
			{
				auto lrank_it = m_ranks.find(ltype);
				if (lrank_it != m_ranks.end())
				{
					lrank_it->second->erase(&ldata);
				}
			}
			ldata = litem;
			for (const pbdb::eranklist ltype : lchangeds)
			{
				auto lrank_it = m_ranks.find(ltype);
				if (lrank_it != m_ranks.end())
				{
					lrank_it->second->insert(&ldata);
				}
			}
		}
		return true;
	}

	void ranklist::add_data(const pbdb::db_ranklist& aitem)
	{
		auto lit = m_maprankitem.try_emplace(aitem.mid()).first;
		rank_item& ltempitem = lit->second;
		ltempitem.m_actorid = aitem.mid();

		for (const auto& lritem : aitem.mitems())
		{
			rank_pair& lpair = ltempitem.m_data[static_cast<pbdb::eranklist>(lritem.first)];
			lpair.m_time = lritem.second.mtime();
			lpair.m_value = lritem.second.mvalue();
		}

		for (const auto& lpair : m_ranks)
		{
			rankset_base& lrank = *lpair.second.get();
			lrank.insert(&ltempitem);
		}
	}

	void ranklist::initdata()
	{
		log_error()->print("actor_ranklist###loaddb_finish {}", data());
		for (auto& lpair : data())
		{
			MODIFIED_CONTINUE_CONST(lpdbranklist, lpair.second);
			add_data(*lpdbranklist);
		}


		std::set<i32_fieldnumber> lfieldset;
		tdb_brief::nsp_cread<actor_ranklist>::instance_readall(
			(actor_ranklist*)get_actor(), 
			pb_field::field_number<pbdb::db_brief>(lfieldset, "mid", "mbase", "mactivityvalues")
		);

		tdb_brief::nsp_cread<actor_ranklist>::instance(get_actor()->id_guid()).set_changedatafun(
				[this]([[maybe_unused]] int64_t aid, const pbdb::db_brief& abrief, bool afirstsynchronize)
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
		auto litor = m_ranks.find(atype);
		if (litor != m_ranks.end())
		{
			rankset_base* lrank = litor->second.get();
			int32_t lcount = lrank->getpage(aroleid, apage, [&apro, this]([[maybe_unused]] int32_t aindex, const rank_item* aitem)
				{
					const pbdb::db_brief* lpbrief = tdb_brief::nsp_cread<actor_ranklist>::instance(get_actor()->id_guid()).getconst(aitem->m_actorid);
					if (lpbrief != nullptr)
					{
						*apro.add_mitems() = *lpbrief;
					}
				});
			apro.set_mcount(lcount);
			apro.set_mrolerank(lrank->role_rank(aroleid));
		}
		else
		{
			apro.set_mcount(0);
			apro.set_mrolerank(-1);
		}		
		return true;
	}

	void ranklist::ranklist_sync(i64_actorid aroleid, pbdb::eranklist atype, [[maybe_unused]] int32_t aactivityid, int32_t apage)
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
		auto litor = m_ranks.find((pbdb::eranklist)arankid);
		if (litor != m_ranks.end())
		{
			litor->second->foreach([&arolerank]([[maybe_unused]] int32_t aindex, const rank_item* aitem)
				{
					arolerank.emplace_back(aitem->m_actorid);
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
		m_ranks.erase(itor);
		erase(arankid);
	}

	void ranklist::rank_add(int32_t arankid)
	{
		std::unique_ptr<rankset_base> ltemp = make_rank::make((pbdb::eranklist)arankid);
		if (ltemp == nullptr)
		{
			return;
		}
		auto lit = m_ranks.try_emplace((pbdb::eranklist)arankid).first;
		lit->second = std::move(ltemp);
		rankset_base* lrank = lit->second.get();
		for (const auto& lpair : m_maprankitem)
		{
			lrank->insert(&lpair.second);
		}
	}
}//namespace ngl
