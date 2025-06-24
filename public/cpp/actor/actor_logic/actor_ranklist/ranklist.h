#pragma once

#include "ttab_specialid.h"
#include "nactor_auto.h"
#include "rank_item.h"
#include "rankset.h"
#include "db.pb.h"

#include <iostream>
#include <sstream>
#include <list>

namespace ngl
{
	class actor_ranklist;

	class ranklist : 
		public tdb_ranklist::db_modular
	{
		ranklist(const ranklist&) = delete;
		ranklist& operator=(const ranklist&) = delete;

		std::map<i64_actorid, rank_item> m_maprankitem;
		std::map<pbdb::eranklist, std::unique_ptr<rankset_base>> m_ranks;
	public:
		ranklist();

		void related_actorid() final;

		bool update_value(pbdb::eranklist atype, rank_item& litem, const pbdb::db_brief& abrief, bool afirstsynchronize);

		bool update_value(const pbdb::db_brief& abrief, bool afirstsynchronize);

		void add_data(const pbdb::db_ranklist& aitem);

		virtual void initdata();

		pbdb::db_ranklist* get(i64_actorid aactorid);
		
		std::shared_ptr<pbnet::PROBUFF_NET_RANKLIST_RESPONSE> get_ranklist(i64_actorid aroleid, pbdb::eranklist atype, int32_t apage);

		void sync_ranklist(i64_actorid aroleid, pbdb::eranklist atype, int32_t aactivityid, int32_t apage);

		void get_rank(int32_t arankid, std::vector<int64_t>& arolerank);

		void remove_rank(int32_t arankid);

		void add_rank(int32_t arankid);
	};
}// namespace ngl

mk_formatter(pbdb::db_ranklist)