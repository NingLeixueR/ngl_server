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
				
		std::shared_ptr<pbnet::PROBUFF_NET_RANKLIST_RESPONSE> get_ranklist(i64_actorid aroleid, pbdb::eranklist atype, int32_t apage);

		void sync_ranklist(i64_actorid aroleid, pbdb::eranklist atype, int32_t aactivityid, int32_t apage);

		void get_rank(int32_t arankid, std::vector<int64_t>& arolerank);

		void remove_rank(int32_t arankid);

		void add_rank(int32_t arankid);
	};
}// namespace ngl

mk_formatter(pbdb::db_ranklist)