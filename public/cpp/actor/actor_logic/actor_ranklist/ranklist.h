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

	class ranklist : public tdb_ranklist::db_modular
	{
		ranklist(const ranklist&) = delete;
		ranklist& operator=(const ranklist&) = delete;

		std::map<i64_actorid, rank_item> m_data;
		std::unique_ptr<rankset_base> m_ranks[pbdb::eranklist::count];
	public:
		ranklist();

		virtual void set_id();

		std::map<nguid, data_modified<pbdb::db_ranklist>>* get_ranklist();

		const pbdb::db_ranklist* get_constrank(i64_actorid aroleid);

		pbdb::db_ranklist* get_rank(i64_actorid aroleid, bool achange = true);

		bool update_value(pbdb::eranklist atype, rank_item& litem, const pbdb::db_brief& abrief, bool afirstsynchronize);

		bool update_value(const pbdb::db_brief& abrief, bool afirstsynchronize);

		void add_data(const pbdb::db_ranklist& aitem);

		virtual void initdata();

		const pbdb::db_ranklist* find(i64_actorid aactorid);

		pbdb::db_ranklist* get(i64_actorid aactorid);
		
		std::shared_ptr<pbnet::PROBUFF_NET_RANKLIST_RESPONSE> get_ranklist(pbdb::eranklist atype, int32_t apage, int32_t aeverynum);

		void sync_ranklist(i64_actorid aroleid, pbdb::eranklist atype, int32_t apage, int32_t aeverynum);
	};
}// namespace ngl