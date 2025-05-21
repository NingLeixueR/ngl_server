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
		std::unique_ptr<rankset_base> m_ranks[pbdb::eranklist::count];
	public:
		ranklist();

		virtual void set_id();

		data_modified<pbdb::db_ranklist>* get_rank(i64_actorid aroleid);

		bool update_value(pbdb::eranklist atype, rank_item& litem, const pbdb::db_brief& abrief, bool afirstsynchronize);

		bool update_value(const pbdb::db_brief& abrief, bool afirstsynchronize);

		void add_data(const pbdb::db_ranklist& aitem);

		virtual void initdata();

		const pbdb::db_ranklist* find(i64_actorid aactorid);

		pbdb::db_ranklist* get(i64_actorid aactorid);
		
		std::shared_ptr<pbnet::PROBUFF_NET_RANKLIST_RESPONSE> get_ranklist(pbdb::eranklist atype, int32_t apage);

		void sync_ranklist(i64_actorid aroleid, pbdb::eranklist atype, int32_t apage);
	};
}// namespace ngl

mk_formatter(pbdb::db_ranklist)