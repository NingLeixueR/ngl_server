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

#include "actor/protocol/nprotocol_template.h"
#include "actor/pb/db.pb.h"
#include "tools/tools.h"

#
#include <map>

namespace ngl
{
	class ndbclient_base;
	class actor_base;
	template <pbdb::ENUM_DB DBTYPE, typename TDBTAB, typename TACTOR>
	class ndbclient;

	class nmanage_dbclient
	{
		using tmap_dbclient = std::map<pbdb::ENUM_DB, ndbclient_base*>;
		actor_base* m_actor = nullptr;
		tmap_dbclient										m_typedbclientmap;
		tmap_dbclient										m_dbclientmap;						//已经加载完的
		std::function<void(pbdb::ENUM_DB, enum_dbstat)>		m_loadfinishfun;					//bool db数据库是否有该数据

		void foreach_function(const std::function<void(ndbclient_base*)>& afun);
	public:
		explicit nmanage_dbclient(actor_base* aactor) :
			m_actor(aactor)
		{}

		void add(ndbclient_base* adbclient, const nguid& aid);

		void set_loadfinish_function(const std::function<void(pbdb::ENUM_DB, enum_dbstat)>& afun);

		void init(ndbclient_base* adbclient, actor_base* aactor, const nguid& aid);

		bool on_load_finish(pbdb::ENUM_DB atype, enum_dbstat astat);

		bool isloadfinish()const;

		template <pbdb::ENUM_DB ENUM, typename TDATA, typename TACTOR>
		ndbclient<ENUM, TDATA, TACTOR>* data(bool aloadfinish);

		void save();

		void del();

		void nscript_push_data();
	};

}//namespace ngl