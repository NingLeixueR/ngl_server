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

#include "nactor_auto.h"
#include "localtime.h"
#include "actor.h"
#include "ncsv.h"

#include <string>
#include <map>

namespace ngl
{
	class actor_brief;

	class briefdb : 
		public tdb_brief::db_modular
	{
		briefdb(const briefdb&) = delete;
		briefdb& operator=(const briefdb&) = delete;

	public:
		briefdb() = default;
		virtual ~briefdb() = default;

		void related_actorid() final
		{
			set_actorid(nguid::make());
		}

		// 没有就添加
		void update_brief(const pbdb::db_brief* adata)
		{
			auto& ldata = get(adata->mid());
			*ldata.get() = *adata;
		}

		virtual void initdata()
		{
			log_error()->print("actor_brief###loaddb_finish\n {}", data());
		}

		void update(const std::vector<pbdb::db_brief>& m_vecinfo)
		{
			for (const pbdb::db_brief& item : m_vecinfo)
			{
				update_brief(&item);
			}
		}
	};
}// namespace ngl

mk_formatter(pbdb::db_brief)