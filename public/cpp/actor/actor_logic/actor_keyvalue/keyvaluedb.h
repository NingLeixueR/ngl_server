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
#include "manage_csv.h"
#include "localtime.h"
#include "actor.h"

#include <string>
#include <map>

namespace ngl
{
	class actor_keyvalue;

	class keyvaluedb : 
		public tdb_keyvalue::db_modular
	{
		keyvaluedb(const keyvaluedb&) = delete;
		keyvaluedb& operator=(const keyvaluedb&) = delete;

	public:
		keyvaluedb() = default;

		void related_actorid() final
		{
			set_actorid(nguid::make());
		}

		template <typename T>
		bool keyvalue(nguid aid, T& adata)
		{
			data_modified<pbdb::db_keyvalue>& ldata = get(aid);
			data_modified_return_getconst(lpdkeyvalue, ldata, false);
			adata = tools::lexical_cast<T>(lpdkeyvalue->mvalue());
			return true;
		}

		template <typename T>
		bool keyvalue_add(nguid aid, const T& adata)
		{
			pbdb::db_keyvalue ldata;
			ldata.set_mid(aid);
			ldata.set_mvalue(tools::lexical_cast<std::string>(adata));
			keyvalue_update(&ldata);
			return true;
		}

		// 没有就添加
		void keyvalue_update(const pbdb::db_keyvalue* adata)
		{
			data_modified<pbdb::db_keyvalue>& lpdata = get(adata->mid());
			*lpdata.get() = *adata;
		}

		virtual void initdata()
		{
			log_error()->print("actor_keyvalue###loaddb_finish {}", data());
		}

		void update(const std::vector<pbdb::db_keyvalue>& m_vecinfo)
		{
			for (const pbdb::db_keyvalue& item : m_vecinfo)
			{
				keyvalue_update(&item);
			}
		}
	};
}// namespace ngl

mk_formatter(pbdb::db_keyvalue)